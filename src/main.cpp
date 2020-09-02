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

#include <QApplication>
#include <QCommandLineParser>

#include <KAboutData>
#include <KConfig>
#include <KLocalizedContext>
#include <KLocalizedString>

#include <KDBusService>

#include "alarmmodel.h"
#include "kclockdsettings.h"
#include "kclocksettingsadaptor.h"
#include "timermodel.h"
QCommandLineParser *createParser()
{
    QCommandLineParser *parser = new QCommandLineParser;
    parser->addOption(QCommandLineOption(QStringLiteral("page"), i18n("Select opened page"), QStringLiteral("page"), "main"));
    parser->addOption(QCommandLineOption(QStringLiteral("no-powerdevil"), i18n("Don't use PowerDevil for alarms if it is available")));
    parser->addOption(QCommandLineOption(QStringLiteral("daemon"), i18n("Run in background mode")));
    parser->addHelpOption();
    return parser;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("kclockd");
    KAboutData aboutData(QStringLiteral("kclockd"), QStringLiteral("KClock daemon"), QStringLiteral("0.1"), QStringLiteral("KClock daemon"), KAboutLicense::GPL, i18n("© 2020 KDE Community"));
    aboutData.addAuthor(i18n("Devin Lin"), QStringLiteral(), QStringLiteral("espidev@gmail.com"));
    aboutData.addAuthor(i18n("Han Young"), QStringLiteral(), QStringLiteral("hanyoung@protonmail.com"));
    KAboutData::setApplicationData(aboutData);

    // only allow one instance
    KDBusService service(KDBusService::Unique);

    // initialize models
    new KClockSettingsAdaptor(KClockSettings::self());
    QDBusConnection::sessionBus().registerObject("/Settings", KClockSettings::self());

    // start alarm polling
    AlarmModel::instance()->configureWakeups();
    TimerModel::instance();

    return app.exec();
}
