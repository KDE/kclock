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

#include "alarm.h"
#include "alarmmodel.h"
#include "alarmplayer.h"
#include "kclockformat.h"
#include "settingsmodel.h"
#include "stopwatchtimer.h"
#include "timer.h"
#include "timermodel.h"
#include "timezoneselectormodel.h"
#include "utilmodel.h"

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
#include <QQmlDebuggingEnabler>
#include <QQuickWindow>
#include <QStringLiteral>

QCommandLineParser *createParser()
{
    QCommandLineParser *parser = new QCommandLineParser;
    parser->addOption(QCommandLineOption(QStringLiteral("page"), i18n("Select opened page"), QStringLiteral("page"), "main"));
    parser->addHelpOption();
    return parser;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlDebuggingEnabler enabler;

    KLocalizedString::setApplicationDomain("kclock");
    KAboutData aboutData("kclock", "Clock", "0.5.0", "A convergent clock application for Plasma", KAboutLicense::GPL, i18n("© 2020 KDE Community"));
    aboutData.addAuthor(i18n("Devin Lin"), QString(), QStringLiteral("espidev@gmail.com"));
    aboutData.addAuthor(i18n("Han Young"), QString(), QStringLiteral("hanyoung@protonmail.com"));
    KAboutData::setApplicationData(aboutData);

    // only allow one instance
    // KDBusService service(KDBusService::Unique);

    // initialize models
    auto *timeZoneModel = new TimeZoneSelectorModel();

    auto *timeZoneViewModel = new QSortFilterProxyModel();
    timeZoneViewModel->setFilterFixedString("true");
    timeZoneViewModel->setSourceModel(timeZoneModel);
    timeZoneViewModel->setFilterRole(TimeZoneSelectorModel::ShownRole);

    auto *timeZoneFilterModel = new TimeZoneFilterModel(timeZoneModel);
    auto *stopwatchTimer = new StopwatchTimer();
    auto *weekModel = new WeekModel();

    // register QML types
    qmlRegisterType<Alarm>("kclock", 1, 0, "Alarm");
    qmlRegisterType<Timer>("kclock", 1, 0, "Timer");

    QQmlApplicationEngine *engine = new QQmlApplicationEngine();

    engine->rootContext()->setContextObject(new KLocalizedContext(engine));
    // models
    engine->rootContext()->setContextProperty("timeZoneSelectorModel", timeZoneModel);
    engine->rootContext()->setContextProperty("timeZoneShowModel", timeZoneViewModel);
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
