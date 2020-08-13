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

#pragma once

#include <QAbstractListModel>
#include <QObject>

class Alarm;
class AlarmWaitWorker;
class QDBusInterface;
class AlarmModel : public QAbstractListModel
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kclock.AlarmModel")
public:
    explicit AlarmModel(QObject *parent = nullptr);

    enum {
        NameRole = Qt::DisplayRole,
        EnabledRole = Qt::UserRole + 1,
        HoursRole,
        MinutesRole,
        DaysOfWeekRole,
        RingtonePathRole,
        AlarmRole,
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void updateUi();

    bool load();

    Q_SCRIPTABLE void remove(QString uuid);
    Q_INVOKABLE void remove(int index);

signals:
    Q_SCRIPTABLE void alarmChanged();
    Q_SCRIPTABLE void nextAlarm(quint64 nextAlarmTimeStampe);

public slots:
    void wakeupCallback(int cookie); // PowerDevil callback function, not scriptable to distinguish from other slots
    Q_SCRIPTABLE quint64 getNextAlarm();
    Q_SCRIPTABLE void scheduleAlarm();
    Q_SCRIPTABLE void addAlarm(int hours, int minutes, int daysOfWeek, QString name, QString ringtonePath = 0); // in 24 hours units, ringTone could be chosen from a list
private:
    quint64 nextAlarmTime = 0;
    QDBusInterface *m_interface = nullptr;
    int m_token = -1; // token for PowerDevil
                      // https://invent.kde.org/plasma/powerdevil/-/merge_requests/13

    bool m_isPowerDevil = false; // if PowerDevil present in system

    Alarm *alarmToBeRung = nullptr; // the alarm we currently waiting

    QList<Alarm *> alarmsList;
    QThread *m_timerThread = nullptr;
    AlarmWaitWorker *m_worker = nullptr;
};
