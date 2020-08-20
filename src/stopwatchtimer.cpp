/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
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

#include "stopwatchtimer.h"
#include <QDebug>
#include <QTimer>

StopwatchTimer::StopwatchTimer(QObject *parent)
    : QObject(parent)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &StopwatchTimer::updateTime);
}

void StopwatchTimer::updateTime()
{
    emit timeChanged();
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
    emit timeChanged();
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
    return amount >= 10 ? QString::number(amount) : "0" + QString::number(amount);
}

QString StopwatchTimer::secondsDisplay()
{
    long long amount = seconds();
    return amount >= 10 ? QString::number(amount) : "0" + QString::number(amount);
}

QString StopwatchTimer::smallDisplay()
{
    long long amount = small();
    return amount >= 10 ? QString::number(amount) : "0" + QString::number(amount);
}
