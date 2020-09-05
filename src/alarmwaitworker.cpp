/*
 * Copyright 2020   Han Young <hanyoung@protonmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "alarmwaitworker.h"
#include <QDateTime>
#include <QDebug>
#include <poll.h>
#include <sys/timerfd.h>
#include <unistd.h>

AlarmWaitWorker::AlarmWaitWorker(qint64 timestamp)
    : m_timerFd(timerfd_create(CLOCK_REALTIME, 0))
    , m_waitEndTime(timestamp)
{
    connect(this, &AlarmWaitWorker::startWait, this, &AlarmWaitWorker::wait);
}

void AlarmWaitWorker::wait(int waitId)
{
    if (m_waitEndTime < 0)
        return;
    struct itimerspec timerSpec;
    timerSpec.it_value.tv_sec = m_waitEndTime;
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

void AlarmWaitWorker::setNewTime(qint64 timestamp)
{
    m_waitId = std::rand();

    m_waitEndTime = timestamp;
    struct itimerspec timerSpec;
    timerSpec.it_value.tv_sec = m_waitEndTime;
    timerSpec.it_value.tv_nsec = 0;
    timerSpec.it_interval.tv_sec = 0;
    timerSpec.it_interval.tv_nsec = 0;
    timerfd_settime(m_timerFd, TFD_TIMER_ABSTIME, &timerSpec, nullptr); // absolute time

    qDebug() << "start waiting, id:" << m_waitId << " Wait Time: " << timestamp;

    Q_EMIT startWait(m_waitId);
}
