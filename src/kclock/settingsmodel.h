/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "kclocksettingsinterface.h"

#include <QObject>

class SettingsModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(QString alarmSilenceAfterDisplay READ alarmSilenceAfterDisplay WRITE setAlarmSilenceAfterDisplay NOTIFY alarmSilenceChanged)
    Q_PROPERTY(QString alarmSnoozeLengthDisplay READ alarmSnoozeLengthDisplay WRITE setAlarmSnoozeLengthDisplay NOTIFY alarmSnoozedChanged)
    Q_PROPERTY(int alarmSilenceAfter READ alarmSilenceAfter WRITE setAlarmSilenceAfter NOTIFY alarmSilenceChanged)
    Q_PROPERTY(int alarmSnoozeLength READ alarmSnoozeLength WRITE setAlarmSnoozeLength NOTIFY alarmSnoozedChanged)

public:
    static SettingsModel &instance();

    const int &volume() const;
    void setVolume(int volume);

    const QString &alarmSilenceAfterDisplay() const;
    void setAlarmSilenceAfterDisplay(QString str);

    const QString &alarmSnoozeLengthDisplay() const;
    void setAlarmSnoozeLengthDisplay(QString str);

    const int &alarmSilenceAfter() const;
    void setAlarmSilenceAfter(int length);

    const int &alarmSnoozeLength() const;
    void setAlarmSnoozeLength(int length);

Q_SIGNALS:
    void volumeChanged();
    void alarmSilenceChanged();
    void alarmSnoozedChanged();

private Q_SLOTS:
    void updateVolume();
    void updateAlarmSilenceAfter();
    void updateAlarmSnoozeLength();

private:
    SettingsModel();
    LocalKClockSettingsInterface *m_interface;

    QString m_alarmSilenceAfterDisplay;
    QString m_alarmSnoozeLengthDisplay;

    int m_volume;
    int m_alarmSilenceAfter;
    int m_alarmSnoozeLength;
};
