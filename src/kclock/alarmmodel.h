/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "alarm.h"
#include "alarmmodelinterface.h"

#include <QAbstractListModel>
#include <QObject>

class AlarmModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool connectedToDaemon READ connectedToDaemon NOTIFY connectedToDaemonChanged)

public:
    static AlarmModel *instance();

    enum {
        AlarmRole,
    };

    void load();

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void remove(int index);

    Q_INVOKABLE void addAlarm(QString name,
                              int hours,
                              int minutes,
                              int daysOfWeek,
                              QString audioPath,
                              int ringDuration,
                              int snoozeDuration); // in 24 hours units, ringTone could be chosen from a list

    Q_INVOKABLE QString timeToRingFormatted(int hours, int minutes, int daysOfWeek); // for new alarm use

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
