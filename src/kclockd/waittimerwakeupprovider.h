/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "abstractwakeupprovider.h"
#include "alarmwaitworker.h"

#include <QThread>

class WaitTimerWakeupProvider : public AbstractWakeupProvider
{
    Q_OBJECT

public:
    explicit WaitTimerWakeupProvider(QObject *parent = nullptr);
    ~WaitTimerWakeupProvider();

    int scheduleWakeup(quint64 timestamp) override;
    void clearWakeup(int cookie) override;

private:
    void schedule(); // For AlarmWaitWorker use
    void initWorker();

    QList<std::pair<int, quint64>> m_waitWorkerCookies; // <cookie, timestamp>. For AlarmWaitWorker use
    QThread *m_timerThread = nullptr;
    int m_cookie = 1; // For AlarmWaitWorker use
    int m_currentCookie; // For AlarmWaitWorker use

    AlarmWaitWorker *m_worker = nullptr;
};
