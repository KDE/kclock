/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "stopwatchtimer.h"

#include <QDebug>
#include <QTimer>

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

long long StopwatchTimer::elapsedTime()
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

long long StopwatchTimer::minutes()
{
    return elapsedTime() / 1000 / 60;
}

long long StopwatchTimer::seconds()
{
    return elapsedTime() / 1000 - 60 * minutes();
}

long long StopwatchTimer::small()
{
    return elapsedTime() / 10 - 100 * seconds() - 100 * 60 * minutes();
}

QString StopwatchTimer::minutesDisplay()
{
    long long amount = minutes();
    return amount >= 10 ? QString::number(amount) : QStringLiteral("0") + QString::number(amount);
}

QString StopwatchTimer::secondsDisplay()
{
    long long amount = seconds();
    return amount >= 10 ? QString::number(amount) : QStringLiteral("0") + QString::number(amount);
}

QString StopwatchTimer::smallDisplay()
{
    long long amount = small();
    return amount >= 10 ? QString::number(amount) : QStringLiteral("0") + QString::number(amount);
}
