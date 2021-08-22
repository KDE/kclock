/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "alarm.h"
#include "alarmmodel.h"
#include "alarmplayer.h"
#include "kclockformat.h"
#include "savedtimezonesmodel.h"
#include "settingsmodel.h"
#include "stopwatchtimer.h"
#include "timer.h"
#include "timermodel.h"
#include "timerpresetmodel.h"
#include "timezoneselectormodel.h"
#include "utilmodel.h"
#include "version.h"

#include <KAboutData>
#include <KConfig>
#include <KLocalizedContext>
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
    parser->addOption(QCommandLineOption(QStringLiteral("page"), i18n("Select opened page"), QStringLiteral("page"), "main"));
    parser->addVersionOption();
    parser->addHelpOption();
    return parser;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    KLocalizedString::setApplicationDomain("kclock");
    KAboutData aboutData("kclock",
                         "Clock",
                         QStringLiteral(KCLOCK_VERSION_STRING),
                         "A convergent clock application for Plasma",
                         KAboutLicense::GPL,
                         i18n("© 2020-2021 KDE Community"));
    aboutData.addAuthor(i18n("Devin Lin"), QString(), QStringLiteral("espidev@gmail.com"));
    aboutData.addAuthor(i18n("Han Young"), QString(), QStringLiteral("hanyoung@protonmail.com"));
    KAboutData::setApplicationData(aboutData);

    // only allow one instance
    // KDBusService service(KDBusService::Unique);

    // initialize models
    auto *timeZoneModel = new TimeZoneSelectorModel();

    auto *timeZoneFilterModel = new TimeZoneFilterModel(timeZoneModel);
    auto *stopwatchTimer = new StopwatchTimer();
    auto *weekModel = new WeekModel();

    // register QML types
    qmlRegisterType<Alarm>("kclock", 1, 0, "Alarm");
    qmlRegisterType<Timer>("kclock", 1, 0, "Timer");
    qmlRegisterType<SavedTimeZonesModel>("kclock", 1, 0, "SavedTimeZonesModel");
    qmlRegisterSingletonType<TimerPresetModel>("kclock", 1, 0, "TimerPresetModel", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return TimerPresetModel::instance();
    });

    QQmlApplicationEngine *engine = new QQmlApplicationEngine();

    engine->rootContext()->setContextObject(new KLocalizedContext(engine));
    // models
    engine->rootContext()->setContextProperty("timeZoneFilterModel", timeZoneFilterModel);
    engine->rootContext()->setContextProperty("alarmModel", AlarmModel::instance());
    engine->rootContext()->setContextProperty("timerModel", TimerModel::instance());
    engine->rootContext()->setContextProperty("utilModel", UtilModel::instance());
    engine->rootContext()->setContextProperty("stopwatchTimer", stopwatchTimer);
    engine->rootContext()->setContextProperty("alarmPlayer", &AlarmPlayer::instance());
    engine->rootContext()->setContextProperty("kclockFormat", KclockFormat::instance());
    engine->rootContext()->setContextProperty("weekModel", weekModel);
    engine->rootContext()->setContextProperty("settingsModel", &SettingsModel::instance());
    engine->rootContext()->setContextProperty(QStringLiteral("kclockAboutData"), QVariant::fromValue(aboutData));

    engine->load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    {
        QScopedPointer<QCommandLineParser> parser(createParser());
        parser->process(app);
        if (parser->isSet(QStringLiteral("page"))) {
            QVariant page;
            QMetaObject::invokeMethod(engine->rootObjects().first(), "getPage", Q_RETURN_ARG(QVariant, page), Q_ARG(QVariant, parser->value("page")));
            QMetaObject::invokeMethod(engine->rootObjects().first(), "switchToPage", Q_ARG(QVariant, page), Q_ARG(QVariant, 0));
        }
    }
    return app.exec();
}
