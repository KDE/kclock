/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <KLocalizedString>

#include <QAbstractListModel>
#include <QObject>

#include "timermodelinterface.h"

class Timer;
class TimerModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool connectedToDaemon READ connectedToDaemon NOTIFY connectedToDaemonChanged)

public:
    static TimerModel *instance();

    enum {
        TimerRole,
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addNew(int length, QString label, QString commandTimeout);
    Q_INVOKABLE void remove(int index);

    bool connectedToDaemon();
    void setConnectedToDaemon(bool connectedToDaemon);

Q_SIGNALS:
    void connectedToDaemonChanged();

private Q_SLOTS:
    void addTimer(QString uuid);
    void removeTimer(QString uuid);

private:
    explicit TimerModel(QObject *parent = nullptr);

    QList<Timer *> m_timersList;
    OrgKdeKclockTimerModelInterface *m_interface;
    QDBusServiceWatcher *m_watcher;
    bool m_connectedToDaemon = false;
};

