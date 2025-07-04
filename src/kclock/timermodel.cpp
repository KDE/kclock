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
#include <QQmlEngine>
#include <QtGlobal>

TimerModel *TimerModel::instance()
{
    static TimerModel *singleton = new TimerModel();
    return singleton;
}

TimerModel *TimerModel::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine);
    Q_UNUSED(jsEngine);
    auto *model = instance();
    QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);
    return model;
}

TimerModel::TimerModel(QObject *parent)
    : QAbstractListModel{parent}
    , m_interface(new OrgKdeKclockTimerModelInterface(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Timers"), QDBusConnection::sessionBus(), this))
{
    if (m_interface->isValid()) {
        // connect timer signals
        connect(m_interface, SIGNAL(timerAdded(QString)), this, SLOT(addTimer(QString)));
        connect(m_interface, SIGNAL(timerRemoved(QString)), this, SLOT(removeTimer(QString)));
        connect(m_interface, SIGNAL(defaultAudioLocationChanged()), this, SLOT(updateDefaultAudioLocation()));
        updateDefaultAudioLocation();
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
    if (parent.isValid()) {
        return 0;
    }
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

void TimerModel::addNew(int length, const QString &label, bool looping, const QString &commandTimeout)
{
    m_interface->addTimer(length, label, looping, commandTimeout, false);
}

void TimerModel::remove(Timer *timer)
{
    if (timer) {
        // request kclockd to remove timer, which will trigger a signal back to remove it from the UI
        m_interface->removeTimer(timer->uuid().toString());
    }
}

void TimerModel::addTimer(QString uuid)
{
    auto *timer = new Timer(uuid.remove(QRegularExpression(QStringLiteral("[{}-]"))));
    connect(timer, &Timer::runningChanged, this, &TimerModel::runningTimerChanged);

    Timer *oldRunningTimer = runningTimer();
    Q_EMIT beginInsertRows(QModelIndex(), 0, 0);
    m_timersList.insert(0, timer);
    Q_EMIT endInsertRows();
    if (oldRunningTimer != runningTimer()) {
        Q_EMIT runningTimerChanged();
    }
}

void TimerModel::removeTimer(const QString &uuid)
{
    Timer *oldRunningTimer = runningTimer();

    for (int i = 0; i < m_timersList.size(); ++i) {
        if (m_timersList[i]->uuid().toString() == uuid) {
            beginRemoveRows(QModelIndex(), i, i);
            m_timersList.at(i)->deleteLater();
            m_timersList.removeAt(i);
            endRemoveRows();
        }
    }

    if (oldRunningTimer != runningTimer()) {
        Q_EMIT runningTimerChanged();
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

QString TimerModel::defaultAudioLocation() const
{
    return m_defaultAudioLocation;
}

void TimerModel::setDefaultAudioLocation(const QString &location)
{
    m_interface->setProperty("defaultAudioLocation", location);
}

void TimerModel::updateDefaultAudioLocation()
{
    m_defaultAudioLocation = m_interface->defaultAudioLocation();
    Q_EMIT defaultAudioLocationChanged();
}

Timer *TimerModel::runningTimer() const
{
    Timer *runningTimer = nullptr;
    for (Timer *timer : std::as_const(m_timersList)) {
        if (!timer->running()) {
            continue;
        }

        if (!runningTimer) {
            runningTimer = timer;
        } else {
            // More than one running timer, exit.
            runningTimer = nullptr;
            break;
        }
    }
    return runningTimer;
}

#include "moc_timermodel.cpp"
