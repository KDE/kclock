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

#ifndef KIRIGAMICLOCK_SETTINGSMODEL_H
#define KIRIGAMICLOCK_SETTINGSMODEL_H

#include <QObject>
#include <QSettings>

class SettingsModel;
static SettingsModel* settingsInst_;

class SettingsModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool use24HourTime READ use24HourTime WRITE setUse24HourTime NOTIFY propertyChanged)
    Q_PROPERTY(int alarmSilenceAfter READ alarmSilenceAfter WRITE setAlarmSilenceAfter NOTIFY propertyChanged)
    Q_PROPERTY(QString alarmSilenceAfterDisplay READ alarmSilenceAfterDisplay WRITE setAlarmSilenceAfterDisplay NOTIFY propertyChanged)
    Q_PROPERTY(int alarmSnoozeLength READ alarmSnoozeLength WRITE setAlarmSnoozeLength NOTIFY propertyChanged)
    Q_PROPERTY(QString alarmSnoozeLengthDisplay READ alarmSnoozeLengthDisplay WRITE setAlarmSnoozeLengthDisplay NOTIFY propertyChanged)

public:
    static SettingsModel* inst()
    {
        return settingsInst_;
    }
    static void init()
    {
        settingsInst_ = new SettingsModel();
    }
    
    bool use24HourTime();
    void setUse24HourTime(bool val);
    int alarmSilenceAfter();
    void setAlarmSilenceAfter(int duration);
    QString alarmSilenceAfterDisplay();
    void setAlarmSilenceAfterDisplay(QString duration);
    int alarmSnoozeLength();
    void setAlarmSnoozeLength(int snooze);
    QString alarmSnoozeLengthDisplay();
    void setAlarmSnoozeLengthDisplay(QString duration);
    
signals:
    void propertyChanged();
};

#endif
