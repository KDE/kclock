// SPDX-FileCopyrightText: 2020 Han Young <hanyoung@protonmail.com>
// SPDX-FileCopyrightText: 2020-2024 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "stopwatchtimer.h"
#include "utilmodel.h"

#include <QDebug>

const int STOPWATCH_DISPLAY_INTERVAL = 41; // 24fps

StopwatchTimer *StopwatchTimer::instance()
{
    static StopwatchTimer *timer = new StopwatchTimer;
    return timer;
}

StopwatchTimer::StopwatchTimer(QObject *parent)
    : QObject(parent)
    , m_timer{new QTimer{this}}
{
    connect(m_timer, &QTimer::timeout, this, &StopwatchTimer::timeChanged);
}

bool StopwatchTimer::paused()
{
    return m_paused;
}

bool StopwatchTimer::stopped()
{
    return m_stopped;
}

void StopwatchTimer::toggle()
{
    if (m_stopped) {
        // start (from zero)
        m_stopped = false;
        m_paused = false;
        m_timerStartStamp = QDateTime::currentMSecsSinceEpoch();

        Q_EMIT stoppedChanged();
        m_timer->start(STOPWATCH_DISPLAY_INTERVAL);
    } else if (m_paused) {
        // unpause
        m_paused = false;
        m_pausedElapsed += QDateTime::currentMSecsSinceEpoch() - m_pausedStamp;

        m_timer->start(STOPWATCH_DISPLAY_INTERVAL);
    } else {
        // pause
        m_paused = true;
        m_pausedStamp = QDateTime::currentMSecsSinceEpoch();

        m_timer->stop();
    }

    Q_EMIT pausedChanged();
}

void StopwatchTimer::reset()
{
    m_timer->stop();

    m_timerStartStamp = 0;
    m_pausedElapsed = 0;
    m_stopped = true;
    m_paused = false;

    Q_EMIT stoppedChanged();
    Q_EMIT pausedChanged();
    Q_EMIT timeChanged();

    Q_EMIT resetTriggered();
}

long long StopwatchTimer::elapsedTime() const
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    if (m_stopped) {
        return 0;
    } else if (m_paused) {
        return currentTime - m_timerStartStamp - m_pausedElapsed - (currentTime - m_pausedStamp);
    } else {
        return currentTime - m_timerStartStamp - m_pausedElapsed;
    }
}

qint64 StopwatchTimer::hours() const
{
    return UtilModel::instance()->msToHoursPart(elapsedTime());
}

qint64 StopwatchTimer::minutes() const
{
    return UtilModel::instance()->msToMinutesPart(elapsedTime());
}

qint64 StopwatchTimer::seconds() const
{
    return UtilModel::instance()->msToSecondsPart(elapsedTime());
}

qint64 StopwatchTimer::small() const
{
    return UtilModel::instance()->msToSmallPart(elapsedTime());
}

QString StopwatchTimer::hoursDisplay() const
{
    qint64 amount = hours();
    return UtilModel::instance()->displayTwoDigits(amount);
}

QString StopwatchTimer::minutesDisplay() const
{
    // % 60 discards anything above 60 minutes. Not used in minutes() because
    // it may tamper with seconds() and small().
    qint64 amount = minutes() % 60;
    return UtilModel::instance()->displayTwoDigits(amount);
}

QString StopwatchTimer::secondsDisplay() const
{
    qint64 amount = seconds();
    return UtilModel::instance()->displayTwoDigits(amount);
}

QString StopwatchTimer::smallDisplay() const
{
    qint64 amount = small();
    return UtilModel::instance()->displayTwoDigits(amount);
}

#include "moc_stopwatchtimer.cpp"
