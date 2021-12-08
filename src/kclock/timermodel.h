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

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    Q_INVOKABLE void addNew(int length, QString label, QString commandTimeout)
    {
        this->addTimer(length, label, commandTimeout, false);
    };
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE int count();
    Q_INVOKABLE Timer *get(int index);
    bool connectedToDaemon();
    void setConnectedToDaemon(bool connectedToDaemon);

Q_SIGNALS:
    void connectedToDaemonChanged();

private Q_SLOTS:
    void addTimer(QString uuid); // remote add, always justCreated
    void addTimer(int length = 300, QString label = i18n("New timer"), QString commandTimeout = QString{}, bool running = false);
    void removeTimer(QString uuid);

private:
    explicit TimerModel(QObject *parent = nullptr);

    void addTimer(QString uuid, bool justCreated);

    QList<Timer *> m_timersList;
    OrgKdeKclockTimerModelInterface *m_interface;
    QDBusServiceWatcher *m_watcher;
    bool m_connectedToDaemon = false;
};

