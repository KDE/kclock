/*
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *           2020 Devin Lin <espidev@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QCommandLineParser>
#include <QMetaObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlDebuggingEnabler>
#include <QStringLiteral>

#include <KAboutData>
#include <KConfig>
#include <KLocalizedContext>
#include <KLocalizedString>

#ifndef Q_OS_ANRDOID
#include <KDBusService>
#endif

#include "alarms.h"
#include "timermodel.h"
#include "timezoneselectormodel.h"
#include "utilmodel.h"
#include "settingsmodel.h"

QCommandLineParser *createParser()
{
    QCommandLineParser *parser = new QCommandLineParser;
    parser->addOption(QCommandLineOption(QStringLiteral("page"), i18n("Select opened page"), QStringLiteral("page"), "main"));
    parser->addOption(QCommandLineOption(QStringLiteral("daemon"), i18n("Run in background mode")));
    parser->addHelpOption();
    return parser;
}

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlDebuggingEnabler enabler;
    QQmlApplicationEngine engine;

    KLocalizedString::setApplicationDomain("kclock");
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    KAboutData aboutData("kclock", "Clock", "0.1", "Simple clock made in Kirigami", KAboutLicense::GPL, i18n("© 2020 KDE Community"));
    aboutData.addAuthor(i18n("Devin Lin"), QString(), QStringLiteral("espidev@gmail.com"));
    KAboutData::setApplicationData(aboutData);

#ifndef Q_OS_ANRDOID
    // only allow one instance
    KDBusService service(KDBusService::Unique);
    // allow to stay running when last window is closed
    app.setQuitOnLastWindowClosed(false);
#endif
    
    // initialize models
    auto *timeZoneModel = new TimeZoneSelectorModel();

    auto *timeZoneViewModel = new QSortFilterProxyModel();
    timeZoneViewModel->setFilterFixedString("true");
    timeZoneViewModel->setSourceModel(timeZoneModel);
    timeZoneViewModel->setFilterRole(TimeZoneSelectorModel::ShownRole);

    auto *timeZoneFilterModel = new TimeZoneFilterModel(timeZoneModel);
    auto *alarmModel = new AlarmModel();
    auto *utilModel = new UtilModel();
    TimerModel::init();
    SettingsModel::init();

    // register QML types
    qmlRegisterType<Alarm>("kclock", 1, 0, "Alarm");
    qmlRegisterType<Timer>("kclock", 1, 0, "Timer");

    // models
    engine.rootContext()->setContextProperty("timeZoneShowModel", timeZoneViewModel);
    engine.rootContext()->setContextProperty("timeZoneFilterModel", timeZoneFilterModel);
    engine.rootContext()->setContextProperty("alarmModel", alarmModel);
    engine.rootContext()->setContextProperty("timerModel", TimerModel::inst());
    engine.rootContext()->setContextProperty("settingsModel", SettingsModel::inst());
    engine.rootContext()->setContextProperty("utilModel", utilModel);

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    {
        QScopedPointer<QCommandLineParser> parser(createParser());
        parser->process(app);
        QObject *rootObject = engine.rootObjects().first();
        if (parser->isSet(QStringLiteral("page"))) {
            QMetaObject::invokeMethod(rootObject, "switchToPage", Q_ARG(QVariant, parser->value("page")));
        }

#ifndef Q_OS_ANRDOID
        if (!parser->isSet(QStringLiteral("daemon"))) {
            QMetaObject::invokeMethod(rootObject, "show");
        }
        QObject::connect(&service, &KDBusService::activateRequested, rootObject, [=](const QStringList &arguments, const QString &workingDirectory) {
            Q_UNUSED(workingDirectory)
            QMetaObject::invokeMethod(rootObject, "show");
        });
#else
        QMetaObject::invokeMethod(rootObject, "show");
#endif
    }

    return app.exec();
}
