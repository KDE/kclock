/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "addlocationmodel.h"
#include "alarm.h"
#include "alarmmodel.h"
#include "kclockformat.h"
#include "savedlocationsmodel.h"
#include "settingsmodel.h"
#include "stopwatchmodel.h"
#include "stopwatchtimer.h"
#include "timer.h"
#include "timermodel.h"
#include "timerpresetmodel.h"
#include "utilityinterface.h"
#include "utilmodel.h"
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
#include <QMetaObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QStringLiteral>

QCommandLineParser *createParser()
{
    QCommandLineParser *parser = new QCommandLineParser;
    parser->addOption(QCommandLineOption(QStringLiteral("page"), i18n("Select opened page"), QStringLiteral("page"), QStringLiteral("main")));
    parser->addVersionOption();
    parser->addHelpOption();
    return parser;
}

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
    KAboutData::setApplicationData(aboutData);
    KCrash::initialize();

    // ~~~~ DBus setup ~~~~

    // ensure kclockd is up with dbus autostart, any call will do
    QDBusInterface *testInterface = new QDBusInterface(QStringLiteral("org.kde.kclockd"),
                                                       QStringLiteral("/Alarms"),
                                                       QStringLiteral("org.freedesktop.DBus.Introspectable"),
                                                       QDBusConnection::sessionBus(),
                                                       nullptr);
    testInterface->call(QStringLiteral("Introspect"));
    testInterface->deleteLater();

    OrgKdePowerManagementInterface *interface =
        new OrgKdePowerManagementInterface(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Utility"), QDBusConnection::sessionBus());
    interface->keepAlive();
    QObject::connect(&app, &QCoreApplication::aboutToQuit, &app, [interface] {
        interface->canExit();
    });

    // ~~~ Qt application setup ~~~~

    // initialize models
    auto *weekModel = new WeekModel();

    // register QML types
    qmlRegisterType<Alarm>("kclock", 1, 0, "Alarm");
    qmlRegisterType<Timer>("kclock", 1, 0, "Timer");

    qmlRegisterSingletonType<AddLocationSearchModel>("kclock", 1, 0, "AddLocationSearchModel", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return AddLocationSearchModel::instance();
    });
    qmlRegisterSingletonType<SavedLocationsModel>("kclock", 1, 0, "SavedLocationsModel", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return SavedLocationsModel::instance();
    });
    qmlRegisterSingletonType<TimerPresetModel>("kclock", 1, 0, "TimerPresetModel", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return TimerPresetModel::instance();
    });
    qmlRegisterSingletonType<TimerModel>("kclock", 1, 0, "TimerModel", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return TimerModel::instance();
    });
    qmlRegisterSingletonType<AlarmModel>("kclock", 1, 0, "AlarmModel", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return AlarmModel::instance();
    });
    qmlRegisterSingletonType<UtilModel>("kclock", 1, 0, "UtilModel", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return UtilModel::instance();
    });
    qmlRegisterSingletonType<StopwatchTimer>("kclock", 1, 0, "StopwatchModel", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return StopwatchModel::instance();
    });
    qmlRegisterSingletonType<StopwatchTimer>("kclock", 1, 0, "StopwatchTimer", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return StopwatchTimer::instance();
    });
    qmlRegisterSingletonType<KclockFormat>("kclock", 1, 0, "KClockFormat", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return KclockFormat::instance();
    });
    qmlRegisterSingletonType<WeekModel>("kclock", 1, 0, "WeekModel", [weekModel](QQmlEngine *, QJSEngine *) -> QObject * {
        return weekModel;
    });
    qmlRegisterSingletonType<SettingsModel>("kclock", 1, 0, "SettingsModel", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return SettingsModel::instance();
    });

    QQmlApplicationEngine *engine = new QQmlApplicationEngine();
    KLocalization::setupLocalizedContext(engine);

    engine->load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.kclock")));

    // ~~~~ Parse command line arguments ~~~~
    {
        QScopedPointer<QCommandLineParser> parser(createParser());
        parser->process(app);
        if (parser->isSet(QStringLiteral("page"))) {
            QVariant page;
            QMetaObject::invokeMethod(engine->rootObjects().first(),
                                      "getPage",
                                      Q_RETURN_ARG(QVariant, page),
                                      Q_ARG(QVariant, parser->value(QStringLiteral("page"))));
            QMetaObject::invokeMethod(engine->rootObjects().first(), "switchToPage", Q_ARG(QVariant, page), Q_ARG(QVariant, 0));
        }
    }

    return app.exec();
}
