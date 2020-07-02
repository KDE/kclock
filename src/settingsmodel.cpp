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

#include "settingsmodel.h"

bool SettingsModel::use24HourTime()
{
    QSettings settings;
    return settings.value("Global/use24HourTime", false).toBool();
}

void SettingsModel::setUse24HourTime(bool val)
{
    QSettings settings;
    settings.setValue("Global/use24HourTime", val);
    emit propertyChanged();
}

int SettingsModel::alarmSilenceAfter()
{
    QSettings settings;
    return settings.value("Global/alarmSilenceAfter", 60).toInt();
}

void SettingsModel::setAlarmSilenceAfter(int duration)
{
    QSettings settings;
    settings.setValue("Global/alarmSilenceAfter", duration);
    emit propertyChanged();
}

QString SettingsModel::alarmSilenceAfterDisplay()
{
    QSettings settings;
    return settings.value("Global/alarmSilenceAfterDisplay", "1 minute").toString();
}

void SettingsModel::setAlarmSilenceAfterDisplay(QString duration)
{
    QSettings settings;
    settings.setValue("Global/alarmSilenceAfterDisplay", duration);
    emit propertyChanged();
}

int SettingsModel::alarmSnoozeLength()
{
    QSettings settings;
    return settings.value("Global/alarmSnoozeLength", 5).toInt();
}

void SettingsModel::setAlarmSnoozeLength(int snooze)
{
    QSettings settings;
    settings.setValue("Global/alarmSnoozeLength", snooze);
    emit propertyChanged();
}

QString SettingsModel::alarmSnoozeLengthDisplay()
{
    QSettings settings;
    return settings.value("Global/alarmSnoozeLengthDisplay", "5 minutes").toString();
}

void SettingsModel::setAlarmSnoozeLengthDisplay(QString duration)
{
    QSettings settings;
    settings.setValue("Global/alarmSnoozeLengthDisplay", duration);
    emit propertyChanged();
}
