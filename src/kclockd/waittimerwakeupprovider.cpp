/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "waittimerwakeupprovider.h"

WaitTimerWakeupProvider::WaitTimerWakeupProvider(QObject *parent)
    : AbstractWakeupProvider(parent)
    , m_timerThread(nullptr)
    , m_cookie(1)
    , m_worker(nullptr)
{
    initWorker();
}

WaitTimerWakeupProvider::~WaitTimerWakeupProvider()
{
    if (m_worker) {
        m_worker->deleteLater();
    }
}

int WaitTimerWakeupProvider::scheduleWakeup(quint64 timestamp)
{
    m_waitWorkerCookies.append({++m_cookie, timestamp});
    schedule();
    return m_cookie;
}

void WaitTimerWakeupProvider::clearWakeup(int cookie)
{
    for (auto index = m_waitWorkerCookies.begin(); index < m_waitWorkerCookies.end(); ++index) {
        int pairCookie = (*index).first;
        if (cookie == pairCookie) {
            m_waitWorkerCookies.erase(index);
        }
    }

    // ensure that we schedule the next queued wakeup
    schedule();
}

void WaitTimerWakeupProvider::schedule()
{
    if (!m_worker) {
        return;
    }

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

void WaitTimerWakeupProvider::initWorker()
{
    if (!m_timerThread) {
        m_timerThread = new QThread();
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

#include "moc_waittimerwakeupprovider.cpp"
