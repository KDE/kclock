/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "alarm.h"

#include <KStatusNotifierItem>
#include <QObject>

class Alarm;

class AlarmModel : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kclock.AlarmModel")

public:
    static AlarmModel *instance();

    void load();
    void save();
    void configureWakeups(); // needs to be called to start worker thread, or configure powerdevil (called in main)

    Q_SCRIPTABLE void removeAlarm(QString uuid);
    Q_SCRIPTABLE void addAlarm(QString name, int hours, int minutes, int daysOfWeek, QString audioPath, int ringDuration, int snoozeDuration);

Q_SIGNALS:
    Q_SCRIPTABLE void alarmAdded(QString uuid);
    Q_SCRIPTABLE void alarmRemoved(QString uuid);
    Q_SCRIPTABLE void nextAlarm(quint64 nextAlarmTimeStamp); // next alarm wakeup timestamp, or 0 if there are none

public Q_SLOTS:
    Q_SCRIPTABLE quint64 getNextAlarm();
    void scheduleAlarm();
    void wakeupCallback(int cookie);

private Q_SLOTS:
    void updateNotifierItem(quint64 time); // update notify icon in systemtray

private:
    explicit AlarmModel(QObject *parent = nullptr);

    void removeAlarm(int index);
    void initNotifierItem();

    // next scheduled system wakeup for ringing alarms, in unix time
    quint64 m_nextAlarmTime = 0;

    // token for system wakeup call authentication
    int m_cookie = -1;

    // list of alarms that will ring on the next scheduled system wakeup
    QList<Alarm *> alarmsToRing;

    // list of alarms in the model
    QList<Alarm *> m_alarmsList;

    // system tray notifier item
    KStatusNotifierItem *m_item{nullptr};
};
