/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "timermodel.h"

#include "timer.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtGlobal>

#include <KLocalizedString>

TimerModel *TimerModel::instance()
{
    static TimerModel *singleton = new TimerModel();
    return singleton;
}

TimerModel::TimerModel(QObject *parent)
    : QAbstractListModel{parent}
    , m_interface(new OrgKdeKclockTimerModelInterface(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Timers"), QDBusConnection::sessionBus(), this))
{
    if (m_interface->isValid()) {
        // connect timer signals
        connect(m_interface, SIGNAL(timerAdded(QString)), this, SLOT(addTimer(QString)));
        connect(m_interface, SIGNAL(timerRemoved(QString)), this, SLOT(removeTimer(QString)));
    }

    // load timers
    const QStringList timers = m_interface->timers();
    for (const QString &timerId : timers) {
        addTimer(timerId);
    }

    // watch for kclockd's status
    setConnectedToDaemon(m_interface->isValid());
    m_watcher = new QDBusServiceWatcher(QStringLiteral("org.kde.kclockd"), QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForOwnerChange, this);
    connect(m_watcher, &QDBusServiceWatcher::serviceRegistered, this, [this]() -> void {
        setConnectedToDaemon(true);
    });
    connect(m_watcher, &QDBusServiceWatcher::serviceUnregistered, this, [this]() -> void {
        setConnectedToDaemon(false);
    });
}

int TimerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_timersList.size();
}

QVariant TimerModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role)

    if ((index.row() < 0) || (index.row() >= m_timersList.count()))
        return {};

    return QVariant::fromValue(m_timersList.at(index.row()));
}

QHash<int, QByteArray> TimerModel::roleNames() const
{
    return {{TimerRole, "timer"}};
}

void TimerModel::addNew(int length, QString label, QString commandTimeout)
{
    m_interface->addTimer(length, label, commandTimeout, false);
}

void TimerModel::remove(int index)
{
    if (index < 0 || index >= m_timersList.size())
        return;

    // request kclockd to remove timer, which will trigger a signal back to remove it from the UI
    m_interface->removeTimer(m_timersList.at(index)->uuid().toString());
}

void TimerModel::addTimer(QString uuid)
{
    auto *timer = new Timer(uuid.remove(QRegularExpression(QStringLiteral("[{}-]"))));

    Q_EMIT beginInsertRows(QModelIndex(), 0, 0);
    m_timersList.insert(0, timer);
    Q_EMIT endInsertRows();
}

void TimerModel::removeTimer(QString uuid)
{
    for (int i = 0; i < m_timersList.size(); ++i) {
        if (m_timersList[i]->uuid().toString() == uuid) {
            beginRemoveRows(QModelIndex(), i, i);
            m_timersList.at(i)->deleteLater();
            m_timersList.removeAt(i);
            endRemoveRows();
        }
    }
}

bool TimerModel::connectedToDaemon()
{
    return m_connectedToDaemon;
}

void TimerModel::setConnectedToDaemon(bool connectedToDaemon)
{
    if (m_connectedToDaemon != connectedToDaemon) {
        m_connectedToDaemon = connectedToDaemon;
        Q_EMIT connectedToDaemonChanged();
    }
}
