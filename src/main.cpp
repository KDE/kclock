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
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCommandLineParser>
#include <QMetaObject>

#include <QQmlDebuggingEnabler>
#include <KLocalizedString>
#include <KLocalizedContext>
#include <KAboutData>
#include <KConfig>

#include "timezoneselectormodel.h"
#include "alarms.h"
#include "timermodel.h"
#include "utilmodel.h"

QCommandLineParser* createParser()
{
    QCommandLineParser* parser = new QCommandLineParser;
    parser->addOption(QCommandLineOption(QStringLiteral("page"), i18n("Select opened page"), QStringLiteral("page"), "main"));
    parser->addHelpOption();
    return parser;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlDebuggingEnabler enabler;
    QQmlApplicationEngine engine;

    KLocalizedString::setApplicationDomain("kclock");
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    KAboutData aboutData("kclock", "Clock", "0.1", "Simple clock made in Kirigami", KAboutLicense::GPL);
    KAboutData::setApplicationData(aboutData);

    // initialize models
	auto *timeZoneModel = new TimeZoneSelectorModel();

	//auto *timeZoneViewModel = new TimeZoneViewModel(timeZoneModel);
    //timeZoneModel->connect(timeZoneModel, &TimeZoneSelectorModel::dataChanged, timeZoneViewModel, &TimeZoneViewModel::dataChanged);

    auto *timeZoneViewModel = new QSortFilterProxyModel();
    timeZoneViewModel->setFilterFixedString("true");
    timeZoneViewModel->setSourceModel(timeZoneModel);
    timeZoneViewModel->setFilterRole(TimeZoneSelectorModel::ShownRole);

    auto *timeZoneFilterModel = new TimeZoneFilterModel(timeZoneModel);
    auto *alarmModel = new AlarmModel();
    auto *timerModel = new TimerModel();
    auto *utilModel = new UtilModel();

    // register QML types
    qmlRegisterType<Alarm>("kclock", 1, 0, "Alarm");

    // models
	engine.rootContext()->setContextProperty("timeZoneShowModel", timeZoneViewModel);
	engine.rootContext()->setContextProperty("timeZoneFilterModel", timeZoneFilterModel);
    engine.rootContext()->setContextProperty("alarmModel", alarmModel);
    engine.rootContext()->setContextProperty("timerModel", timerModel);
    engine.rootContext()->setContextProperty("utilModel", utilModel);
    
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    {
        QScopedPointer<QCommandLineParser> parser(createParser());
        parser->process(app);
        if(parser->isSet(QStringLiteral("page"))) {
            QObject* rootObject = engine.rootObjects().first();
            QMetaObject::invokeMethod(rootObject, "switchToPage", Q_ARG(QVariant, parser->value("page")));
        }
    }

    return app.exec();
}
