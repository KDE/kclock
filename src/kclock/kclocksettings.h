/*
 * Copyright 2020   Han Young <hanyoung@protonmail.com>
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
#pragma once

#include "kclocksettingsinterface.h"
#include <QObject>

class KClockSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(QString alarmSilenceAfterDisplay READ alarmSilenceAfterDisplay WRITE setAlarmSilenceAfterDisplay NOTIFY alarmSilenceChanged)
    Q_PROPERTY(QString alarmSnoozeLengthDisplay READ alarmSnoozeLengthDisplay WRITE setAlarmSnoozeLengthDisplay NOTIFY alarmSnoozedChanged)
    Q_PROPERTY(int alarmSilenceAfter READ alarmSilenceAfter WRITE setAlarmSilenceAfter NOTIFY alarmSilenceChanged)
    Q_PROPERTY(int alarmSnoozeLength READ alarmSnoozeLength WRITE setAlarmSnoozeLength NOTIFY alarmSnoozedChanged)

public:
    static KClockSettings &instance()
    {
        static KClockSettings singleton;
        return singleton;
    };

    const int &volume() const
    {
        return m_volume;
    }

    void setVolume(int volume)
    {
        m_interface->setProperty("alarmVolume", volume);
    }

    const QString &alarmSilenceAfterDisplay() const
    {
        return m_alarmSilenceAfterDisplay;
    }

    void setAlarmSilenceAfterDisplay(QString str)
    {
        m_alarmSilenceAfterDisplay = str;
    }

    const QString &alarmSnoozeLengthDisplay() const
    {
        return m_alarmSnoozeLengthDisplay;
    }

    void setAlarmSnoozeLengthDisplay(QString str)
    {
        m_alarmSnoozeLengthDisplay = str;
    }

    const int &alarmSilenceAfter() const
    {
        return m_alarmSilenceAfter;
    }

    void setAlarmSilenceAfter(int length)
    {
        m_interface->setProperty("alarmSilenceAfter", length);
    }

    void setAlarmSnoozeLength(int length)
    {
        m_interface->setProperty("alarmSnoozeLength", length);
    }

    const int &alarmSnoozeLength() const
    {
        return m_alarmSnoozeLength;
    }

Q_SIGNALS:
    void volumeChanged();
    void alarmSilenceChanged();
    void alarmSnoozedChanged();

private Q_SLOTS:
    void updateVolume();
    void updateAlarmSilenceAfter();
    void updateAlarmSnoozeLength();

private:
    KClockSettings();
    LocalKClockSettingsInterface *m_interface;

    QString m_alarmSilenceAfterDisplay;
    QString m_alarmSnoozeLengthDisplay;

    int m_volume;
    int m_alarmSilenceAfter;
    int m_alarmSnoozeLength;
};
