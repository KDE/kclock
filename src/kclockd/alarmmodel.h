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
    Q_SCRIPTABLE void addAlarm(int hours, int minutes, int daysOfWeek, QString name, QString ringtonePath); // in 24 hours units

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
    void removeAlarm(int index);

    explicit AlarmModel(QObject *parent = nullptr);

    void initNotifierItem();

    quint64 m_nextAlarmTime = 0;
    int m_cookie = -1; // token for wakeup call auth
    QList<Alarm *> alarmsToRing; // the alarms that will ring on next wakeup

    QList<Alarm *> m_alarmsList;
    KStatusNotifierItem *m_item{nullptr};
};
