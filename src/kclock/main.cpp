/*
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *           2020 Devin Lin <espidev@gmail.com>
 *                Han Young <hanyoung@protonmail.com>
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
#include <malloc.h>

#include <QApplication>
#include <QCommandLineParser>
#include <QMetaObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QQmlDebuggingEnabler>
#include <QStringLiteral>

#include <KAboutData>
#include <KConfig>
#include <KLocalizedContext>
#include <KLocalizedString>

#include <KDBusService>

#include "alarmmodel.h"
#include "alarmplayer.h"
#include "alarms.h"
#include "kclockformat.h"
#include "kclocksettings.h"
#include "stopwatchtimer.h"
#include "timermodel.h"
#include "timezoneselectormodel.h"
#include "utilmodel.h"

static QQmlApplicationEngine *engine = nullptr;

static StopwatchTimer *stopwatchTimer = nullptr;
static KclockFormat *kclockFormat = nullptr;
static WeekModel *weekModel = nullptr;
static QSortFilterProxyModel *timeZoneViewModel = nullptr;
static KClockSettings *settingsModel = nullptr;
static TimeZoneFilterModel *timeZoneFilterModel = nullptr;

QCommandLineParser *createParser()
{
    QCommandLineParser *parser = new QCommandLineParser;
    parser->addOption(QCommandLineOption(QStringLiteral("page"), i18n("Select opened page"), QStringLiteral("page"), "main"));
    parser->addOption(QCommandLineOption(QStringLiteral("no-powerdevil"), i18n("Don't use PowerDevil for alarms if it is available")));
    parser->addOption(QCommandLineOption(QStringLiteral("daemon"), i18n("Run in background mode")));
    parser->addHelpOption();
    return parser;
};

QObject* initGui() {

    if (!UtilModel::instance()->applicationLoaded()) {
        qDebug() << "construct QmlEngine";
        engine = new QQmlApplicationEngine();
        UtilModel::instance()->setApplicationLoaded(true);
        engine->rootContext()->setContextObject(new KLocalizedContext(engine));
        // models
        engine->rootContext()->setContextProperty("timeZoneShowModel", timeZoneViewModel);
        engine->rootContext()->setContextProperty("timeZoneFilterModel", timeZoneFilterModel);
        engine->rootContext()->setContextProperty("alarmModel", AlarmModel::instance());
        engine->rootContext()->setContextProperty("timerModel", TimerModel::instance());
        engine->rootContext()->setContextProperty("settingsModel", settingsModel);
        engine->rootContext()->setContextProperty("utilModel", UtilModel::instance());
        engine->rootContext()->setContextProperty("stopwatchTimer", stopwatchTimer);
        engine->rootContext()->setContextProperty("alarmPlayer", &AlarmPlayer::instance());
        engine->rootContext()->setContextProperty("kclockFormat", kclockFormat);
        engine->rootContext()->setContextProperty("weekModel", weekModel);

        engine->load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    }
    return engine->rootObjects().first();
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlDebuggingEnabler enabler;

    KLocalizedString::setApplicationDomain("kclock");
    KAboutData aboutData("kclock", "Clock", "0.1", "Simple clock made in Kirigami", KAboutLicense::GPL, i18n("© 2020 KDE Community"));
    aboutData.addAuthor(i18n("Devin Lin"), QString(), QStringLiteral("espidev@gmail.com"));
    aboutData.addAuthor(i18n("Han Young"), QString(), QStringLiteral("hanyoung@protonmail.com"));
    KAboutData::setApplicationData(aboutData);

    // only allow one instance
    KDBusService service(KDBusService::Unique);
    // allow to stay running when last window is closed
    app.setQuitOnLastWindowClosed(false);

    // initialize models
    auto *timeZoneModel = new TimeZoneSelectorModel();

    timeZoneViewModel = new QSortFilterProxyModel();
    timeZoneViewModel->setFilterFixedString("true");
    timeZoneViewModel->setSourceModel(timeZoneModel);
    timeZoneViewModel->setFilterRole(TimeZoneSelectorModel::ShownRole);

    timeZoneFilterModel = new TimeZoneFilterModel(timeZoneModel);
    stopwatchTimer = new StopwatchTimer();
    kclockFormat = new KclockFormat();
    weekModel = new WeekModel();
    settingsModel = new KClockSettings();

    // register QML types
    qmlRegisterType<Alarm>("kclock", 1, 0, "Alarm");
    qmlRegisterType<Timer>("kclock", 1, 0, "Timer");

    {
        QScopedPointer<QCommandLineParser> parser(createParser());
        parser->process(app);
        if (parser->isSet(QStringLiteral("page"))) {
            QMetaObject::invokeMethod(initGui(), "switchToPage", Q_ARG(QVariant, parser->value("page")));
        }
        if (parser->isSet(QStringLiteral("no-powerdevil"))) {
            qDebug() << "No PowerDevil option set, disabling PowerDevil usage";
            AlarmModel::instance()->setUsePowerDevil(false);
        }
        if (!parser->isSet(QStringLiteral("daemon"))) {
            QMetaObject::invokeMethod(initGui(), "show");
        }
        
        QObject::connect(&service, &KDBusService::activateRequested, [=] {
            QMetaObject::invokeMethod(initGui(), "show");
        });
    }

    QObject::connect(&app, &QApplication::lastWindowClosed, [=]{
        if (engine) {
            qDebug() << "last window closed, delete QmlEngine";
            UtilModel::instance()->setApplicationLoaded(false);
            
            engine->clearComponentCache();
            engine->collectGarbage();
            delete engine;
            engine = nullptr;
            malloc_trim(0);
        }
    });
    // start alarm polling
    AlarmModel::instance()->configureWakeups();

    return app.exec();
}
