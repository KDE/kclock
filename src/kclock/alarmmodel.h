/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KCLOCK_ALARMSMODEL_H
#define KCLOCK_ALARMSMODEL_H

#include "alarm.h"
#include "alarmmodelinterface.h"

#include <QAbstractListModel>
#include <QObject>

class AlarmModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool connectedToDaemon READ connectedToDaemon NOTIFY connectedToDaemonChanged)

public:
    static AlarmModel *instance()
    {
        static AlarmModel *singleton = new AlarmModel();
        return singleton;
    }

    enum {
        NameRole = Qt::DisplayRole,
        EnabledRole = Qt::UserRole + 1,
        HoursRole,
        MinutesRole,
        DaysOfWeekRole,
        AlarmRole,
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void updateUi();

    Q_INVOKABLE void remove(int index);

    Q_INVOKABLE void
    addAlarm(int hours, int minutes, int daysOfWeek, QString name, QString ringtonePath = 0); // in 24 hours units, ringTone could be chosen from a list

    Q_INVOKABLE QString timeToRingFormated(int hours, int minutes, int daysOfWeek); // for new alarm use

    bool connectedToDaemon();
    void setConnectedToDaemon(bool connectedToDaemon);

Q_SIGNALS:
    void connectedToDaemonChanged();

private Q_SLOTS:
    void addAlarm(QString uuid);
    void removeAlarm(QString uuid);

private:
    org::kde::kclock::AlarmModel *m_interface;
    QDBusServiceWatcher *m_watcher;
    bool m_connectedToDaemon = false;

    explicit AlarmModel(QObject *parent = nullptr);
    QList<Alarm *> alarmsList;
};

#endif // KCLOCK_ALARMSMODEL_H
