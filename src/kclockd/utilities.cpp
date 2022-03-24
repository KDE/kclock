/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "utilities.h"

#include <QApplication>
#include <QDBusReply>
#include <QDBusServiceWatcher>
#include <QDebug>
#include <QThread>
#include <QTimer>

// manually disable powerdevil, even if found
static bool noPowerDevil = false;

void Utilities::disablePowerDevil(bool disable)
{
    noPowerDevil = disable;
}

Utilities &Utilities::instance()
{
    static Utilities singleton;
    return singleton;
}

Utilities::Utilities(QObject *parent)
    : QObject(parent)
    , m_interface(new QDBusInterface(POWERDEVIL_SERVICE_NAME,
                                     QStringLiteral("/org/kde/Solid/PowerManagement"),
                                     QStringLiteral("org.kde.Solid.PowerManagement"),
                                     QDBusConnection::sessionBus(),
                                     this))
    , m_hasPowerDevil(false)
    , m_timer(new QTimer(this))
{
    // TODO: It'd be nice to be able to have the daemon off if no alarms/timers are running.
    // However, with the current implementation, the client continuously thinks it is off.
    //     connect(m_timer, &QTimer::timeout, this, [this] {
    //         if (m_activeTimerCount <= 0) {
    //             QApplication::exit();
    //         }
    //     });

    // if PowerDevil is present, we can rely on PowerDevil to track time, otherwise we do it ourself
    if (m_interface->isValid() && !noPowerDevil) {
        m_hasPowerDevil = hasWakeup();
    }

    bool success = QDBusConnection::sessionBus().registerObject(QStringLiteral("/Utility"),
                                                                QStringLiteral("org.kde.PowerManagement"),
                                                                this,
                                                                QDBusConnection::ExportScriptableSlots);
    qDebug() << "Registered on DBus:" << success;

    if (hasPowerDevil()) {
        qDebug() << "PowerDevil found, using it for time tracking.";
    } else {
        initWorker();
        qDebug() << "PowerDevil not found, using wait worker thread for time tracking.";
    }

    if (!noPowerDevil) { // do not watch for powerdevil if we explicitly turned it off
        auto m_watcher = new QDBusServiceWatcher{POWERDEVIL_SERVICE_NAME,
                                                 QDBusConnection::sessionBus(),
                                                 QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration};
        connect(m_watcher, &QDBusServiceWatcher::serviceRegistered, this, [this]() {
            qDebug() << "PowerDevil found on DBus";
            m_hasPowerDevil = hasWakeup();
            if (m_hasPowerDevil && m_timerThread)
                m_timerThread->quit();

            Q_EMIT needsReschedule();
        });
        connect(m_watcher, &QDBusServiceWatcher::serviceUnregistered, this, [this]() {
            m_hasPowerDevil = false;
            initWorker();

            Q_EMIT needsReschedule();
        });
    }

    // exit after 1 min if nothing happens
    m_timer->start(60 * 1000);
}

bool Utilities::hasPowerDevil()
{
    return m_hasPowerDevil;
}

int Utilities::scheduleWakeup(quint64 timestamp)
{
    if (hasPowerDevil()) {
        QDBusReply<uint> reply = m_interface->call(QStringLiteral("scheduleWakeup"), QStringLiteral("org.kde.kclockd"), QDBusObjectPath("/Utility"), timestamp);
        if (reply.isValid()) {
            m_powerDevilCookies.append(reply.value());
        } else {
            qDebug() << "Invalid reply, error: " << reply.error();
        }
        return reply.value();
    } else {
        m_waitWorkerCookies.append({++m_cookie, timestamp});
        schedule();
        return m_cookie;
    }
}

void Utilities::clearWakeup(int cookie)
{
    if (hasPowerDevil()) {
        auto index = m_powerDevilCookies.indexOf(cookie);
        if (index != -1) {
            m_interface->call(QStringLiteral("clearWakeup"), cookie);
            m_powerDevilCookies.removeAt(index);
        }
    } else {
        for (auto index = m_waitWorkerCookies.begin(); index < m_waitWorkerCookies.end(); ++index) {
            int pairCookie = (*index).first;
            if (cookie == pairCookie) {
                m_waitWorkerCookies.erase(index);
            }
        }

        // ensure that we schedule the next queued wakeup
        schedule();
    }
}

void Utilities::wakeupCallback(int cookie)
{
    qDebug() << "Received wakeup callback.";
    auto index = m_powerDevilCookies.indexOf(cookie);

    if (index == -1) {
        // something must be wrong here, return and do nothing
        qDebug() << "Callback ignored (wrong cookie).";
    } else {
        // remove token
        m_powerDevilCookies.removeAt(index);
        Q_EMIT wakeup(cookie);
    }
}

void Utilities::schedule()
{
    auto eternity = std::numeric_limits<unsigned long long>::max();
    auto minTime = eternity;

    for (auto tuple : m_waitWorkerCookies) {
        if (minTime > tuple.second) {
            minTime = tuple.second;
            m_currentCookie = tuple.first;
        }
    }

    if (minTime != eternity) { // only schedule worker if we have something to wait on
        m_worker->setNewTime(minTime); // Unix uses int64 internally for time
    }
}

void Utilities::initWorker()
{
    if (!m_timerThread) {
        m_timerThread = new QThread(this);
        m_worker = new AlarmWaitWorker();
        m_worker->moveToThread(m_timerThread);
        connect(m_worker, &AlarmWaitWorker::finished, this, [this] {
            // notify time is up
            Q_EMIT wakeup(m_currentCookie);
            clearWakeup(m_currentCookie);
        });
    }
    m_timerThread->start();
}

bool Utilities::hasWakeup()
{
    QDBusMessage m = QDBusMessage::createMethodCall(QStringLiteral("org.kde.Solid.PowerManagement"),
                                                    QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                    QStringLiteral("org.freedesktop.DBus.Introspectable"),
                                                    QStringLiteral("Introspect"));
    QDBusReply<QString> result = QDBusConnection::sessionBus().call(m);

    return result.isValid() && result.value().indexOf(QStringLiteral("scheduleWakeup")) >= 0;
}

void Utilities::exitAfterTimeout()
{
    m_timer->start(30000);
}

void Utilities::incfActiveCount()
{
    m_activeTimerCount++;
}

void Utilities::decfActiveCount()
{
    m_activeTimerCount--;
    exitAfterTimeout();
}

void Utilities::wakeupNow()
{
    QDBusMessage wakeupCall = QDBusMessage::createMethodCall(QStringLiteral("org.kde.Solid.PowerManagement"),
                                                             QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                             QStringLiteral("org.kde.Solid.PowerManagement"),
                                                             QStringLiteral("wakeup"));
    QDBusConnection::sessionBus().call(wakeupCall);
}

// hack, use timer count to keep alive
void Utilities::keepAlive()
{
    incfActiveCount();
}

void Utilities::canExit()
{
    decfActiveCount();
}
