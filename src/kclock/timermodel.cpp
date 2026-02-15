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
    // connect timer signals
    connect(m_interface, &OrgKdeKclockTimerModelInterface::timerAdded, this, &TimerModel::addTimer);
    connect(m_interface, &OrgKdeKclockTimerModelInterface::timerRemoved, this, &TimerModel::removeTimer);
    connect(m_interface, &OrgKdeKclockTimerModelInterface::defaultAudioLocationChanged, this, [this](const QString &location) {
        m_defaultAudioLocation = location;
        Q_EMIT defaultAudioLocationChanged();
    });

    // watch for kclockd
    m_watcher = new QDBusServiceWatcher(QStringLiteral("org.kde.kclockd"), QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForOwnerChange, this);
    connect(m_watcher, &QDBusServiceWatcher::serviceRegistered, this, [this]() -> void {
        load();
    });
    connect(m_watcher, &QDBusServiceWatcher::serviceUnregistered, this, [this]() -> void {
        clear();
        setStatus(Status::NotConnected);
        setErrorString({});
    });

    load();
}

void TimerModel::load()
{
    // load from dbus
    setStatus(Status::Loading);
    setErrorString({});

    auto timersCall = m_interface->timers();
    auto *watcher = new QDBusPendingCallWatcher(timersCall, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher] {
        QDBusPendingReply<QStringList> reply = *watcher;

        if (reply.isError()) {
            qWarning() << "Failed to fetch timers" << reply.error().name() << reply.error().message();
            clear();
            if (reply.error().type() == QDBusError::ServiceUnknown) {
                setStatus(Status::NotConnected);
            } else {
                setErrorString(reply.error().message());
                setStatus(Status::Error);
            }
        } else {
            beginResetModel();
            qDeleteAll(m_timersList);
            m_timersList.clear();

            Timer *oldRunningTimer = runningTimer();
            const QStringList timerIds = reply.value();
            for (QString timerId : timerIds) {
                auto *timer = new Timer(timerId.remove(QRegularExpression(QStringLiteral("[{}-]"))));
                connect(timer, &Timer::runningChanged, this, &TimerModel::runningTimerChanged);
                m_timersList.append(timer);
            }

            endResetModel();
            if (oldRunningTimer != runningTimer()) {
                Q_EMIT runningTimerChanged();
            }
            setStatus(Status::Ready);

            // TODO async.
            const QString defaultAudioLocation = m_interface->defaultAudioLocation();
            if (m_defaultAudioLocation != defaultAudioLocation) {
                m_defaultAudioLocation = defaultAudioLocation;
                Q_EMIT defaultAudioLocationChanged();
            }
        }

        watcher->deleteLater();
    });
}

TimerModel::Status TimerModel::status() const
{
    return m_status;
}

void TimerModel::setStatus(TimerModel::Status status)
{
    if (m_status != status) {
        m_status = status;
        Q_EMIT statusChanged(status);
    }
}

QString TimerModel::errorString() const
{
    return m_errorString;
}

void TimerModel::setErrorString(const QString &errorString)
{
    if (m_errorString != errorString) {
        m_errorString = errorString;
        Q_EMIT errorStringChanged(errorString);
    }
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

Timer *TimerModel::timerByUuid(const QString &uuid) const
{
    for (Timer *a : m_timersList) {
        if (a->uuid().toString() == uuid)
            return a;
    }
    return nullptr;
}

void TimerModel::addTimer(QString uuid)
{
    auto *timer = new Timer(uuid.remove(QRegularExpression(QStringLiteral("[{}-]"))));
    connect(timer, &Timer::runningChanged, this, &TimerModel::runningTimerChanged);

    Timer *oldRunningTimer = runningTimer();
    beginInsertRows(QModelIndex(), 0, 0);
    m_timersList.insert(0, timer);
    endInsertRows();
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

void TimerModel::clear()
{
    beginResetModel();
    qDeleteAll(m_timersList);
    m_timersList.clear();
    endResetModel();
}

QString TimerModel::defaultAudioLocation() const
{
    return m_defaultAudioLocation;
}

void TimerModel::setDefaultAudioLocation(const QString &location)
{
    m_interface->setDefaultAudioLocation(location);
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
