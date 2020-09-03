/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
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

#include "utilmodel.h"

#include <QString>
#include <QTimeZone>

#include <KLocalizedString>

QString UtilModel::getCurrentTimeZoneName()
{
    return QTimeZone::systemTimeZoneId();
}

bool UtilModel::applicationLoaded()
{
    return m_applicationLoaded;
}

void UtilModel::setApplicationLoaded(bool applicationLoaded)
{
    if (applicationLoaded != m_applicationLoaded) {
        m_applicationLoaded = applicationLoaded;
        Q_EMIT applicationLoadedChanged();
    }
}

long long UtilModel::calculateNextRingTime(int hours, int minutes, int daysOfWeek, int snooze)
{
    // get the time that the alarm will ring on the day
    QTime alarmTime = QTime(hours, minutes, 0).addSecs(snooze);

    QDateTime date = QDateTime::currentDateTime();

    if (daysOfWeek == 0) {              // alarm does not repeat (no days of the week are specified)
        if (alarmTime >= date.time()) { // alarm occurs later today
            return QDateTime(date.date(), alarmTime).toSecsSinceEpoch();
        } else { // alarm occurs on the next day
            return QDateTime(date.date().addDays(1), alarmTime).toSecsSinceEpoch();
        }
    } else { // repeat alarm
        bool first = true;

        // keeping looping forward a single day until the day of week is accepted
        while (((daysOfWeek & (1 << (date.date().dayOfWeek() - 1))) == 0) // check day
               || (first && (alarmTime < date.time())))                   // check time if the current day is accepted (keep looping forward if alarmTime has passed)
        {
            date = date.addDays(1); // go forward a day
            first = false;
        }

        return QDateTime(date.date(), alarmTime).toSecsSinceEpoch();
    }
};

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
        return i18n("Alarm will be rung within a minute");
    } else {
        return i18n("Alarm will be rung after %1", arg);
    }
}
