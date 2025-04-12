/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kclockdsettings.h"
#include "utilityinterface.h"
#include "version.h"

#include <KAboutData>
#include <KConfig>
#include <KCrash>
#include <KLocalizedContext>
#include <KLocalizedQmlContext>
#include <KLocalizedString>

#include <KDBusService>

#include <QApplication>
#include <QCommandLineParser>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QMetaObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QStringLiteral>

int main(int argc, char *argv[])
{
    // set default style
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }
    // if using org.kde.desktop, ensure we use kde style if possible
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORMTHEME")) {
        qputenv("QT_QPA_PLATFORMTHEME", "kde");
    }

    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("kclock");
    KAboutData aboutData(QStringLiteral("kclock"),
                         QStringLiteral("Clock"),
                         QStringLiteral(KCLOCK_VERSION_STRING),
                         i18n("Set alarms and timers, use a stopwatch, and manage world clocks"),
                         KAboutLicense::GPL,
                         i18n("© 2020-2024 KDE Community"));
    aboutData.setBugAddress("https://bugs.kde.org/describecomponents.cgi?product=KClock");
    aboutData.addAuthor(i18n("Devin Lin"), QString(), QStringLiteral("devin@kde.org"));
    aboutData.addAuthor(i18n("Han Young"), QString(), QStringLiteral("hanyoung@protonmail.com"));
    aboutData.addAuthor(i18n("Kai Uwe Broulik"), i18nc("Author", "Desktop Integration"), QStringLiteral("kde@broulik.de"));
    KAboutData::setApplicationData(aboutData);
    KCrash::initialize();

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    QCommandLineOption pageOption(QStringLiteral("page"), i18n("Select opened page"), QStringLiteral("page"));
    parser.addOption(pageOption);
    QCommandLineOption newWindowOption(QStringLiteral("new-window"), i18n("Explicitly open a new Clock window"));
    parser.addOption(newWindowOption);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    // ~~~~ DBus setup ~~~~

    const KDBusService::StartupOption serviceOptions = parser.isSet(newWindowOption) ? KDBusService::Multiple : KDBusService::Unique;
    KDBusService service(serviceOptions);

    // ensure kclockd is up with dbus autostart, any call will do
    {
        const QDBusMessage testMessage = QDBusMessage::createMethodCall(QStringLiteral("org.kde.kclockd"),
                                                                        QStringLiteral("/"),
                                                                        QStringLiteral("org.freedesktop.DBus.Peer"),
                                                                        QStringLiteral("Ping"));
        QDBusConnection::sessionBus().call(testMessage);
    }

    OrgKdePowerManagementInterface *interface =
        new OrgKdePowerManagementInterface(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Utility"), QDBusConnection::sessionBus());
    interface->keepAlive();
    QObject::connect(&app, &QCoreApplication::aboutToQuit, &app, [interface] {
        interface->canExit();
    });

    // ~~~ Qt application setup ~~~~

    QQmlApplicationEngine engine;
    KLocalization::setupLocalizedContext(&engine);

    // ~~~~ Parse command line arguments ~~~~
    const QString initialPage = parser.isSet(pageOption) ? parser.value(pageOption) : QStringLiteral("Time");
    engine.setInitialProperties({{QStringLiteral("initialPage"), initialPage}});
    engine.loadFromModule(QStringLiteral("org.kde.kclock"), QStringLiteral("Main"));

    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.kclock")));

    QQuickWindow *mainWindow = qobject_cast<QQuickWindow *>(engine.rootObjects().first());
    if (!mainWindow) {
        qFatal() << "Failed to create main window";
    }

    QObject::connect(&service,
                     &KDBusService::activateRequested,
                     mainWindow,
                     [&parser, mainWindow](const QStringList &arguments, const QString &workingDirectory) {
                         Q_UNUSED(workingDirectory);
                         parser.parse(arguments);

                         if (parser.isSet(QStringLiteral("page"))) {
                             const QString pageName = parser.value(QStringLiteral("page"));
                             QVariant page;
                             QMetaObject::invokeMethod(mainWindow, "getPage", qReturnArg(page), QVariant(pageName));
                             if (page.isNull()) {
                                 qWarning() << "Unknown page to switch to" << pageName;
                             } else {
                                 QMetaObject::invokeMethod(mainWindow, "switchToPage", QVariant(page), QVariant(0));
                             }
                         }

                         mainWindow->requestActivate();
                     });

    return app.exec();
}
