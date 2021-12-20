/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "utilmodel.h"
#include "settingsmodel.h"

#include <QLocale>
#include <QStandardPaths>
#include <QString>
#include <QTimeZone>
#include <QUrl>

#include <KLocalizedString>

UtilModel *UtilModel::instance()
{
    static UtilModel *singleton = new UtilModel;
    return singleton;
}

QString UtilModel::getDefaultAlarmFileLocation()
{
    return QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("sounds/freedesktop/stereo/alarm-clock-elapsed.oga")))
        .path();
}

QString UtilModel::getCurrentTimeZoneName()
{
    return QString::fromStdString(QTimeZone::systemTimeZoneId().toStdString());
}

long long UtilModel::calculateNextRingTime(int hours, int minutes, int daysOfWeek, int snooze)
{
    // get the time that the alarm will ring on the day
    QTime alarmTime = QTime(hours, minutes, 0).addSecs(snooze);

    QDateTime date = QDateTime::currentDateTime();

    if (daysOfWeek == 0) { // alarm does not repeat (no days of the week are specified)
        if (alarmTime >= date.time()) { // alarm occurs later today
            return QDateTime(date.date(), alarmTime).toSecsSinceEpoch();
        } else { // alarm occurs on the next day
            return QDateTime(date.date().addDays(1), alarmTime).toSecsSinceEpoch();
        }
    } else { // repeat alarm
        bool first = true;

        // keeping looping forward a single day until the day of week is accepted
        while (((daysOfWeek & (1 << (date.date().dayOfWeek() - 1))) == 0) // check day
               || (first && (alarmTime < date.time()))) // check time if the current day is accepted (keep looping forward if alarmTime has passed)
        {
            date = date.addDays(1); // go forward a day
            first = false;
        }

        return QDateTime(date.date(), alarmTime).toSecsSinceEpoch();
    }
}

QString UtilModel::timeToRingFormatted(const long long &timestamp)
{
    auto remaining = timestamp - QDateTime::currentSecsSinceEpoch();
    int day = remaining / (24 * 3600);
    int hour = remaining / 3600 - day * 24;
    int minute = remaining / 60 - day * 24 * 60 - hour * 60;
    QString arg;
    if (day > 0) {
        arg += i18np("%1 day", "%1 days", day);
    }
    if (hour > 0) {
        if (day > 0 && minute > 0) {
            arg += i18n(", ");
        } else if (day > 0) {
            arg += i18n(" and ");
        }
        arg += i18np("%1 hour", "%1 hours", hour);
    }
    if (minute > 0) {
        if (day > 0 || hour > 0) {
            arg += i18n(" and ");
        }
        arg += i18np("%1 minute", "%1 minutes", minute);
    }

    if (day <= 0 && hour <= 0 && minute <= 0) {
        return i18n("Alarm will ring in under a minute");
    } else {
        return i18n("Alarm will ring in %1", arg);
    }
}

QString UtilModel::timeFormat()
{
    return use24HourTime() ? QStringLiteral("hh:mm") : QStringLiteral("hh:mm ap");
}

bool UtilModel::use24HourTime()
{
    QString timeFormat = SettingsModel::instance()->timeFormat();
    if (timeFormat == QStringLiteral("SystemDefault")) {
        return isLocale24HourTime();
    } else {
        return timeFormat == QStringLiteral("24Hour");
    }
}

bool UtilModel::isLocale24HourTime()
{
    return QLocale::system().timeFormat(QLocale::ShortFormat).toLower().indexOf(QStringLiteral("ap")) == -1;
}
