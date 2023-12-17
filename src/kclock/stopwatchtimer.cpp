/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "stopwatchtimer.h"

#include <QDebug>

StopwatchTimer::StopwatchTimer(QObject *parent)
    : QObject(parent)
    , m_timer{new QTimer{this}}
{
    connect(m_timer, &QTimer::timeout, this, &StopwatchTimer::updateTime);
}

void StopwatchTimer::updateTime()
{
    Q_EMIT timeChanged();
}

void StopwatchTimer::toggle()
{
    if (stopped) { // start (from zero)
        stopped = false;
        paused = false;

        timerStartStamp = QDateTime::currentMSecsSinceEpoch();
        pausedElapsed = 0;

        m_timer->start(m_interval);
    } else if (paused) { // unpause
        paused = false;

        pausedElapsed += QDateTime::currentMSecsSinceEpoch() - pausedStamp;

        m_timer->start(m_interval);
    } else { // pause
        paused = true;
        pausedStamp = QDateTime::currentMSecsSinceEpoch();
        m_timer->stop();
    }
}

void StopwatchTimer::reset()
{
    m_timer->stop();
    pausedElapsed = 0;
    stopped = true;
    paused = false;
    Q_EMIT timeChanged();
}

long long StopwatchTimer::elapsedTime() const
{
    long long cur = QDateTime::currentMSecsSinceEpoch();
    if (stopped) {
        return 0;
    } else if (paused) {
        return cur - timerStartStamp - pausedElapsed - (cur - pausedStamp);
    } else {
        return cur - timerStartStamp - pausedElapsed;
    }
}

long long StopwatchTimer::hours() const
{
    return elapsedTime() / 3600000;
}

long long StopwatchTimer::minutes() const
{
    return elapsedTime() / 1000 / 60;
}

long long StopwatchTimer::seconds() const
{
    return elapsedTime() / 1000 - 60 * minutes();
}

long long StopwatchTimer::small() const
{
    return elapsedTime() / 10 - 100 * seconds() - 100 * 60 * minutes();
}

QString StopwatchTimer::displayZeroOrAmount(const int &amount)
{
    return QStringLiteral("%1").arg(amount, 2, 10, QLatin1Char('0'));
}

QString StopwatchTimer::hoursDisplay() const
{
    long long amount = hours();
    return displayZeroOrAmount(amount);
}

QString StopwatchTimer::minutesDisplay() const
{
    // % 60 discards anything above 60 minutes. Not used in minutes() because
    // it may tamper with seconds() and small().
    long long amount = minutes() % 60;
    return displayZeroOrAmount(amount);
}

QString StopwatchTimer::secondsDisplay() const
{
    long long amount = seconds();
    return displayZeroOrAmount(amount);
}

QString StopwatchTimer::smallDisplay() const
{
    long long amount = small();
    return displayZeroOrAmount(amount);
}
