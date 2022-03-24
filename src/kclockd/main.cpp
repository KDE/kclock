/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "alarmmodel.h"
#include "kclockdsettings.h"
#include "kclocksettingsadaptor.h"
#include "timermodel.h"
#include "utilities.h"
#include "version.h"
#include "xdgportal.h"

#include <KAboutData>
#include <KConfig>
#include <KLocalizedContext>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>

#include <KDBusService>

QCommandLineParser *createParser()
{
    QCommandLineParser *parser = new QCommandLineParser;
    parser->addOption(QCommandLineOption(QStringLiteral("no-powerdevil"), i18n("Don't use PowerDevil for alarms if it is available")));
    parser->addHelpOption();
    return parser;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("kclockd");
    KAboutData aboutData(QStringLiteral("kclockd"),
                         QStringLiteral("KClock daemon"),
                         QStringLiteral(KCLOCK_VERSION_STRING),
                         QStringLiteral("KClock daemon"),
                         KAboutLicense::GPL,
                         i18n("© 2020-2022 KDE Community"));
    aboutData.addAuthor(i18n("Devin Lin"), QLatin1String(), QStringLiteral("devin@kde.org"));
    aboutData.addAuthor(i18n("Han Young"), QLatin1String(), QStringLiteral("hanyoung@protonmail.com"));
    KAboutData::setApplicationData(aboutData);

    // ~~~~ Parse command line arguments ~~~~
    {
        QScopedPointer<QCommandLineParser> parser(createParser());
        parser->process(app);

        if (parser->isSet(QStringLiteral("no-powerdevil"))) {
            Utilities::disablePowerDevil(true);
        }
    }

    // only allow one instance
    KDBusService service(KDBusService::Unique);

    qDebug() << "Starting kclockd" << KCLOCK_VERSION_STRING;

    // call org.freedesktop.portal.Background for autostart in flatpak
    XDGPortal *portalInterface = new XDGPortal();
    portalInterface->requestBackground();

    // initialize models
    new KClockSettingsAdaptor(KClockSettings::self());
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/Settings"), KClockSettings::self());

    // save config
    QObject::connect(KClockSettings::self(), &KClockSettings::timeFormatChanged, KClockSettings::self(), &KClockSettings::save);

    // start alarm polling
    AlarmModel::instance()->configureWakeups();
    TimerModel::instance();

    return app.exec();
}
