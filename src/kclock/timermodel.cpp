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
    : m_interface(new OrgKdeKclockTimerModelInterface(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Timers"), QDBusConnection::sessionBus(), this))
{
    if (m_interface->isValid()) {
        connect(m_interface, SIGNAL(timerAdded(QString)), this, SLOT(addTimer(QString)));
        connect(m_interface, SIGNAL(timerRemoved(QString)), this, SLOT(removeTimer(QString)));
    }
    setConnectedToDaemon(m_interface->isValid());

    const QStringList timers = m_interface->timers();

    for (const QString &timerId : timers) {
        addTimer(timerId, false);
    }

    // watch for kclockd
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
    if ((index.row() < 0) || (index.row() >= m_timersList.count()))
        return {};

    return QVariant::fromValue(m_timersList.at(index.row()));
}

QHash<int, QByteArray> TimerModel::roleNames() const
{
    return {{TimerRole, "timer"}};
}

void TimerModel::addTimer(int length, QString label, QString commandTimeout, bool running)
{
    m_interface->addTimer(length, label, commandTimeout, running);
}

void TimerModel::addNew(int length, QString label, QString commandTimeout)
{
    this->addTimer(length, label, commandTimeout, false);
}

void TimerModel::remove(int index)
{
    if (index < 0 || index >= m_timersList.size())
        return;

    beginRemoveRows(QModelIndex(), index, index);
    m_interface->removeTimer(m_timersList.at(index)->uuid().toString());
    m_timersList.at(index)->deleteLater();

    m_timersList.removeAt(index);
    endRemoveRows();
}

void TimerModel::addTimer(QString uuid)
{
    this->addTimer(uuid, true);
}

void TimerModel::addTimer(QString uuid, bool justCreated)
{
    auto *timer = new Timer(uuid.remove(QRegularExpression(QStringLiteral("[{}-]"))), justCreated);

    Q_EMIT beginInsertRows(QModelIndex(), 0, 0);
    m_timersList.insert(0, timer);
    Q_EMIT endInsertRows();
}

void TimerModel::removeTimer(QString uuid)
{
    auto index = 0;
    for (auto timer : m_timersList) {
        if (timer->uuid().toString() == uuid) {
            break;
        }
        ++index;
    }

    // don't need to check index out of bound, remove(index) takes care of that
    remove(index);
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
