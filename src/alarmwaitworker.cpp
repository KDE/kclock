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

void AlarmWaitWorker::wait()
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
        emit error();
        return;
    }

    qDebug() << "waiting end";

    emit finished();
}

void AlarmWaitWorker::setNewTime(qint64 timestamp)
{
    m_waitEndTime = timestamp;
    struct itimerspec timerSpec;
    timerSpec.it_value.tv_sec = m_waitEndTime;
    timerSpec.it_value.tv_nsec = 0;
    timerSpec.it_interval.tv_sec = 0;
    timerSpec.it_interval.tv_nsec = 0;
    timerfd_settime(m_timerFd, TFD_TIMER_ABSTIME, &timerSpec, nullptr); // absolute time

    qDebug() << "start waiting: " << QDateTime::fromSecsSinceEpoch(timestamp).toLocalTime().toString();

    emit startWait();
}
