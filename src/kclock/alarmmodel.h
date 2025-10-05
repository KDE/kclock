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

#include <qqmlregistration.h>

class QJSEngine;
class QQmlEngine;

class AlarmModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)

public:
    static AlarmModel *instance();
    static AlarmModel *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    enum class Status {
        None,
        Ready,
        Loading,
        NotConnected,
        Error,
    };
    Q_ENUM(Status)

    enum Roles {
        AlarmRole = Qt::UserRole,
    };

    void load();

    Status status() const;
    Q_SIGNAL void statusChanged(Status status);

    QString errorString() const;
    Q_SIGNAL void errorStringChanged(const QString &errorString);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void remove(int index);

    Q_INVOKABLE void addAlarm(const QString &name,
                              int hours,
                              int minutes,
                              int daysOfWeek,
                              const QString &audioPath,
                              int ringDuration,
                              int snoozeDuration); // in 24 hours units, ringTone could be chosen from a list

    Q_INVOKABLE QString timeToRingFormatted(int hours, int minutes, int daysOfWeek); // for new alarm use

private Q_SLOTS:
    void addAlarmInternal(QString uuid);
    void removeAlarm(const QString &uuid);

private:
    void clear();

    void setStatus(Status status);
    void setErrorString(const QString &errorString);

    org::kde::kclock::AlarmModel *m_interface;
    QDBusServiceWatcher *m_watcher;

    Status m_status = Status::None;
    QString m_errorString;

    explicit AlarmModel(QObject *parent = nullptr);
    QList<Alarm *> alarmsList;
};
