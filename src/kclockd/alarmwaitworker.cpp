/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "alarmwaitworker.h"

#include <QDateTime>
#include <QDebug>

#include <poll.h>
#include <sys/timerfd.h>
#include <unistd.h>

AlarmWaitWorker::AlarmWaitWorker(quint64 timestamp)
    : m_timerFd{timerfd_create(CLOCK_REALTIME, 0)}
    , m_waitEndTime{timestamp}
{
    connect(this, &AlarmWaitWorker::startWait, this, &AlarmWaitWorker::wait);
}

void AlarmWaitWorker::wait(int waitId)
{
    if (m_waitEndTime == 0) {
        return;
    }

    struct itimerspec timerSpec;
    timerSpec.it_value.tv_sec = static_cast<long>(m_waitEndTime);
    timerSpec.it_value.tv_nsec = 0;
    timerSpec.it_interval.tv_sec = 0;
    timerSpec.it_interval.tv_nsec = 0;

    timerfd_settime(m_timerFd, TFD_TIMER_ABSTIME, &timerSpec, nullptr); // absolute time

    struct pollfd fd; // only one fd
    fd.fd = m_timerFd;
    fd.events = POLLIN | POLLPRI;

    poll(&fd, 1, -1);

    if (fd.revents & POLLNVAL) {
        Q_EMIT error();
        return;
    }
    // ensure that there is one wakeup and not multiple
    if (waitId != this->m_waitId) {
        qDebug() << "cancel old wakeup" << waitId;
        return;
    }

    qDebug() << "waiting end" << waitId;

    Q_EMIT finished();
}

void AlarmWaitWorker::setNewTime(quint64 timestamp)
{
    m_waitId = std::rand();

    m_waitEndTime = timestamp;
    struct itimerspec timerSpec;
    timerSpec.it_value.tv_sec = static_cast<long>(m_waitEndTime);
    timerSpec.it_value.tv_nsec = 0;
    timerSpec.it_interval.tv_sec = 0;
    timerSpec.it_interval.tv_nsec = 0;
    timerfd_settime(m_timerFd, TFD_TIMER_ABSTIME, &timerSpec, nullptr); // absolute time

    qDebug() << "start waiting, id:" << m_waitId << " Wait Time:" << timestamp;

    Q_EMIT startWait(m_waitId);
}
