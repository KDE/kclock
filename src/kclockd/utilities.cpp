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
Utilities::Utilities(QObject *parent)
    : QObject(parent)
    , m_interface(new QDBusInterface(POWERDEVIL_SERVICE_NAME,
                                     QStringLiteral("/org/kde/Solid/PowerManagement"),
                                     QStringLiteral("org.kde.Solid.PowerManagement"),
                                     QDBusConnection::sessionBus(),
                                     this))
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, [this] {
        if (m_activeTimerCount <= 0) {
            QApplication::exit();
        }
    });

    // if PowerDevil is present, we can rely on PowerDevil to track time, otherwise we do it ourself
    if (m_interface->isValid()) {
        // test Plasma 5.20 PowerDevil schedule wakeup feature
        m_hasPowerDevil = hasWakeup();
    }

    bool success = QDBusConnection::sessionBus().registerObject(QStringLiteral("/Utility"),
                                                                QStringLiteral("org.kde.PowerManagement"),
                                                                this,
                                                                QDBusConnection::ExportScriptableSlots);
    qDebug() << "Register Utility on DBus: " << success;
    if (this->hasPowerDevil()) {
        qDebug() << "PowerDevil found, using it for time tracking.";
    } else {
        initWorker();

        qDebug() << "PowerDevil not found, using wait worker thread for time tracking.";
    }

    auto m_watcher = new QDBusServiceWatcher{POWERDEVIL_SERVICE_NAME,
                                             QDBusConnection::sessionBus(),
                                             QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration};
    connect(m_watcher, &QDBusServiceWatcher::serviceRegistered, this, [this]() {
        qDebug() << "powerdevil found";
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

    // exit after 1 min if nothing happens
    m_timer->start(60 * 1000);
}

int Utilities::scheduleWakeup(quint64 timestamp)
{
    if (this->hasPowerDevil()) {
        QDBusReply<uint> reply = m_interface->call(QStringLiteral("scheduleWakeup"), QStringLiteral("org.kde.kclockd"), QDBusObjectPath("/Utility"), timestamp);
        if (reply.isValid()) {
            m_cookies.append(reply.value());
        } else {
            qDebug() << "invalid reply, error: " << reply.error();
        }
        return reply.value();
    } else {
        m_list.append({++m_cookie, timestamp});
        this->schedule();
        return m_cookie;
    }
}

void Utilities::clearWakeup(int cookie)
{
    if (this->hasPowerDevil()) {
        auto index = m_cookies.indexOf(cookie);
        if (index != -1) {
            m_interface->call(QStringLiteral("clearWakeup"), cookie);
            m_cookies.removeAt(index);
        }
    } else {
        auto index = 0;
        for (const auto &tuple : std::as_const(m_list)) {
            if (cookie == std::get<0>(tuple)) {
                break;
            }
            ++index;
        }
        m_list.removeAt(index);

        this->schedule();
    }
}

void Utilities::wakeupCallback(int cookie)
{
    qDebug() << "wakeup callback";
    auto index = m_cookies.indexOf(cookie);

    if (index == -1) {
        // something must be wrong here, return and do nothing
        qDebug() << "callback ignored (wrong cookie)";
        return;
    } else {
        // remove token
        m_cookies.removeAt(index);
        Q_EMIT wakeup(cookie);
    }
}

void Utilities::schedule()
{
    auto minTime = std::numeric_limits<unsigned long long>::max();

    for (const auto &tuple : std::as_const(m_list)) {
        if (minTime > std::get<1>(tuple)) {
            minTime = std::get<1>(tuple);
            m_currentCookie = std::get<0>(tuple);
        }
    }
    m_worker->setNewTime(minTime); // Unix uses int64 internally for time, if we don't have anything to wait, we wait to year 2262 A.D.
}
void Utilities::initWorker()
{
    if (!m_timerThread) {
        m_timerThread = new QThread(this);
        m_worker = new AlarmWaitWorker();
        m_worker->moveToThread(m_timerThread);
        connect(m_worker, &AlarmWaitWorker::finished, this, [this] {
            // notify time is up
            Q_EMIT this->wakeup(m_currentCookie);
            this->clearWakeup(m_currentCookie);
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

    if (result.isValid() && result.value().indexOf(QStringLiteral("scheduleWakeup")) >= 0) { // have this feature
        return true;
    } else {
        return false;
    }
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

// hack, use timer count to keep alive
void Utilities::keepAlive()
{
    incfActiveCount();
}

void Utilities::canExit()
{
    decfActiveCount();
}
