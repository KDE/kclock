/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "timer.h"
#include "utilmodel.h"

/* ~ Timer ~ */
Timer::Timer(const QString &uuid, QObject *parent)
    : QObject{parent}
    , m_interface{new OrgKdeKclockTimerInterface(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Timers/") + uuid, QDBusConnection::sessionBus(), this)}
    , m_animationTimer{new QTimer(this)}
    , m_completedTimeTimer{new QTimer(this)}
{
    // update the timer UI at set intervals, rather than as fast as possible
    connect(m_animationTimer, &QTimer::timeout, [this] {
        m_elapsed = m_interface->elapsed();
        Q_EMIT elapsedChanged();
    });

    connect(m_completedTimeTimer, &QTimer::timeout, [this] {
        m_timeCompleted = m_interface->timeCompleted();
        Q_EMIT timeCompletedChanged();
    });

    if (m_interface->isValid()) {
        connect(m_interface, &OrgKdeKclockTimerInterface::lengthChanged, this, &Timer::updateLength);
        connect(m_interface, &OrgKdeKclockTimerInterface::timeCompletedChanged, this, &Timer::updateTimeCompleted);
        connect(m_interface, &OrgKdeKclockTimerInterface::labelChanged, this, &Timer::updateLabel);
        connect(m_interface, &OrgKdeKclockTimerInterface::commandTimeoutChanged, this, &Timer::updateCommandTimeout);
        connect(m_interface, &OrgKdeKclockTimerInterface::runningChanged, this, &Timer::updateRunning);
        connect(m_interface, &OrgKdeKclockTimerInterface::loopingChanged, this, &Timer::updateLooping);
        connect(m_interface, &OrgKdeKclockTimerInterface::ringingChanged, this, &Timer::updateRinging);

        m_uuid = QUuid(m_interface->uuid().value());
        updateLength();
        updateElapsed();
        updateTimeCompleted();
        updateLabel();
        updateCommandTimeout();
        updateRunning(); // start animation
        updateLooping();
        updateRinging();
    }
}

const QUuid &Timer::uuid()
{
    return m_uuid;
}

int Timer::length() const
{
    return m_length;
}

QString Timer::lengthPretty() const
{
    return toPretty(m_length);
}

void Timer::setLength(int length)
{
    m_interface->setLength(length);
}

int Timer::timeCompleted() const
{
    return m_timeCompleted;
}

QString Timer::timeCompletedPretty() const
{
    return QLocale::system().toString(QDateTime::fromSecsSinceEpoch(m_timeCompleted).toLocalTime().time(), UtilModel::instance()->timeFormat());
}

int Timer::elapsed() const
{
    return m_elapsed;
}

QString Timer::elapsedPretty() const
{
    return toPretty(m_elapsed);
}

QString Timer::remainingPretty() const
{
    return toPretty(m_length - m_elapsed);
}

QString Timer::toPretty(int len) const
{
    qint64 hours = len / 60 / 60, minutes = len / 60 - hours * 60, seconds = len - hours * 60 * 60 - minutes * 60;
    return QStringLiteral("%1:%2:%3").arg(hours).arg(minutes, 2, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0'));
}

QString Timer::label() const
{
    return m_label;
}

void Timer::setLabel(const QString &label)
{
    m_interface->setLabel(label);
}

QString Timer::commandTimeout() const
{
    return m_commandTimeout;
}

void Timer::setCommandTimeout(const QString &commandTimeout)
{
    m_interface->setCommandTimeout(commandTimeout);
}

bool Timer::running() const
{
    return m_running;
}

bool Timer::looping() const
{
    return m_looping;
}

void Timer::setLooping(bool looping)
{
    m_interface->setLooping(looping);
}

bool Timer::ringing() const
{
    return m_ringing;
}

void Timer::toggleRunning()
{
    m_interface->toggleRunning();
}

void Timer::toggleLooping()
{
    m_interface->toggleLooping();
}

void Timer::reset()
{
    m_interface->reset();

    m_elapsed = m_interface->elapsed();
    Q_EMIT elapsedChanged();
}

void Timer::addMinute()
{
    int newLength = length() + 60;
    m_interface->setLength(newLength);
}

void Timer::dismiss()
{
    m_interface->dismiss();
}

void Timer::updateLength()
{
    m_length = m_interface->length();
    Q_EMIT lengthChanged();
}

void Timer::updateTimeCompleted()
{
    m_timeCompleted = m_interface->timeCompleted();
    Q_EMIT timeCompletedChanged();
}

void Timer::updateElapsed()
{
    m_elapsed = m_interface->elapsed();
    Q_EMIT elapsedChanged();
}

void Timer::updateLabel()
{
    m_label = m_interface->label();
    Q_EMIT labelChanged();
}

void Timer::updateCommandTimeout()
{
    m_commandTimeout = m_interface->commandTimeout();
    Q_EMIT commandTimeoutChanged();
}

void Timer::updateRunning()
{
    m_running = m_interface->running();
    Q_EMIT runningChanged();

    animation(m_running);

    updateElapsed();
}

void Timer::updateLooping()
{
    m_looping = m_interface->looping();
    Q_EMIT loopingChanged();
}

void Timer::updateRinging()
{
    m_ringing = m_interface->ringing();
    Q_EMIT ringingChanged();
}

void Timer::animation(bool start)
{
    if (start) {
        m_animationTimer->start(250);
        m_completedTimeTimer->start(60000);
    } else {
        m_animationTimer->stop();
        m_completedTimeTimer->stop();
    }
}

#include "moc_timer.cpp"
