// SPDX-FileCopyrightText: 2020 Han Young <hanyoung@protonmail.com>
// SPDX-FileCopyrightText: 2020-2024 Devin Lin <devin@kde.org>
// SPDX-FileCopyrightText: 2025 Kai Uwe Broulik <kde@broulik.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "stopwatchtimer.h"
#include "utilmodel.h"

#include <QDebug>
#include <QQmlEngine>

#include "wayland/pipshellsurface.h"

const int STOPWATCH_DISPLAY_INTERVAL = 41; // 24fps

StopwatchTimer *StopwatchTimer::instance()
{
    static StopwatchTimer *timer = new StopwatchTimer;
    return timer;
}

StopwatchTimer *StopwatchTimer::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine);
    Q_UNUSED(jsEngine);
    auto *timer = instance();
    QQmlEngine::setObjectOwnership(timer, QQmlEngine::CppOwnership);
    return timer;
}

StopwatchTimer::StopwatchTimer(QObject *parent)
    : QObject(parent)
{
    m_reportTimer.setInterval(STOPWATCH_DISPLAY_INTERVAL);
    m_reportTimer.callOnTimeout(this, &StopwatchTimer::timeChanged);
}

bool StopwatchTimer::paused() const
{
    return !m_elapsedTimer.isValid() && m_pausedTime.has_value();
}

bool StopwatchTimer::stopped() const
{
    return !m_elapsedTimer.isValid() && !m_pausedTime.has_value();
}

void StopwatchTimer::toggle()
{
    if (m_elapsedTimer.isValid()) {
        // pause.
        m_pausedTime = m_pausedTime.value_or(0) + m_elapsedTimer.elapsed();
        m_elapsedTimer.invalidate();
        Q_EMIT pausedChanged();
        m_reportTimer.stop();
        Q_EMIT timeChanged();
    } else {
        // start or resume.
        m_elapsedTimer.start();
        if (m_pausedTime) {
            Q_EMIT pausedChanged();
        } else {
            Q_EMIT stoppedChanged();
        }
        m_reportTimer.start();
        Q_EMIT timeChanged();
    }
}

void StopwatchTimer::reset()
{
    m_elapsedTimer.invalidate();
    m_pausedTime.reset();
    m_reportTimer.stop();

    Q_EMIT stoppedChanged();
    Q_EMIT pausedChanged();
    Q_EMIT timeChanged();

    Q_EMIT resetTriggered();
}

long long StopwatchTimer::elapsedTime() const
{
    long long time = 0;
    if (m_elapsedTimer.isValid()) {
        time += m_elapsedTimer.elapsed();
    }
    if (m_pausedTime) {
        time += *m_pausedTime;
    }
    return time;
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

QString StopwatchTimer::display() const
{
    // Only show hours if we have passed an hour.
    if (hours() > 0) {
        return QStringLiteral("%1:%2:%3").arg(hoursDisplay(), minutesDisplay(), secondsDisplay());
    } else {
        return QStringLiteral("%1:%2").arg(minutesDisplay(), secondsDisplay());
    }
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
