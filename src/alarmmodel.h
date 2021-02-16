/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
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

#ifndef KCLOCKD_ALARMMODEL_H
#define KCLOCKD_ALARMMODEL_H

#include <QObject>
#include <KStatusNotifierItem>

#include "alarm.h"

class Alarm;
class AlarmModel : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kclock.AlarmModel")
public:
    static AlarmModel *instance()
    {
        static AlarmModel *singleton = new AlarmModel();
        return singleton;
    }

    void load();
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

    KStatusNotifierItem *m_notifierItem = nullptr;

    quint64 m_nextAlarmTime = 0;
    int m_cookie = -1;             // token for wakeup call auth
    QList<Alarm *> alarmsToBeRung; // the alarms that will be rung on next wakeup

    QList<Alarm *> m_alarmsList;
};

#endif // KCLOCKD_ALARMMODEL_H
