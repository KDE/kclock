/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "timer.h"

#include <QTimer>

/* ~ Timer ~ */
Timer::Timer()
{
}
Timer::Timer(QString uuid, bool justCreated)
    : m_justCreated(justCreated)
    , m_interface(new OrgKdeKclockTimerInterface(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Timers/") + uuid, QDBusConnection::sessionBus(), this))

{
    if (m_interface->isValid()) {
        m_uuid = QUuid(m_interface->getUUID());
        m_label = m_interface->label();
        m_length = m_interface->length();
        m_commandTimeout = m_interface->commandTimeout();
        m_looping = m_interface->looping();
        m_running = m_interface->running();
        m_elapsed = m_interface->elapsed();
        connect(m_interface, &OrgKdeKclockTimerInterface::lengthChanged, this, &Timer::updateLength);
        connect(m_interface, &OrgKdeKclockTimerInterface::labelChanged, this, &Timer::updateLabel);
        connect(m_interface, &OrgKdeKclockTimerInterface::commandTimeoutChanged, this, &Timer::updateCommandTimeout);
        connect(m_interface, &OrgKdeKclockTimerInterface::loopingChanged, this, &Timer::updateLooping);
        connect(m_interface, &OrgKdeKclockTimerInterface::runningChanged, this, &Timer::updateRunning);

        updateRunning(); // start animation
    }
}

const QUuid &Timer::uuid()
{
    return m_uuid;
}

void Timer::toggleRunning()
{
    m_interface->toggleRunning();
}

void Timer::toggleLooping()
{
    m_interface->toggleLooping();
}

QString Timer::lengthPretty() const
{
    qint64 len = m_length, hours = len / 60 / 60, minutes = len / 60 - hours * 60, seconds = len - hours * 60 * 60 - minutes * 60;
    return QString::number(hours) + QStringLiteral(":") + QString::number(minutes).rightJustified(2, QLatin1Char('0')) + QStringLiteral(":")
        + QString::number(seconds).rightJustified(2, QLatin1Char('0'));
}

QString Timer::elapsedPretty() const
{
    qint64 len = m_elapsed, hours = len / 60 / 60, minutes = len / 60 - hours * 60, seconds = len - hours * 60 * 60 - minutes * 60;
    return QString::number(hours) + QStringLiteral(":") + QString::number(minutes).rightJustified(2, QLatin1Char('0')) + QStringLiteral(":")
        + QString::number(seconds).rightJustified(2, QLatin1Char('0'));
}

int Timer::length() const
{
    return m_length;
}
void Timer::setLength(int length)
{
    m_interface->setLength(length);
}
const int &Timer::elapsed() const
{
    return m_elapsed;
}
const QString &Timer::label() const
{
    return m_label;
}
void Timer::setLabel(QString label)
{
    m_interface->setLabel(label);
}
const QString &Timer::commandTimeout() const
{
    return m_commandTimeout;
}
const bool &Timer::looping() const
{
    return m_looping;
}
void Timer::setCommandTimeout(QString commandTimeout)
{
    m_interface->setCommandTimeout(commandTimeout);
}
const bool &Timer::running() const
{
    return m_running;
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
    updateLength();
    Q_EMIT propertyChanged();
}

void Timer::updateLength()
{
    m_length = m_interface->length();
    Q_EMIT propertyChanged();
}

void Timer::updateLabel()
{
    m_label = m_interface->label();
    Q_EMIT propertyChanged();
}
void Timer::updateRunning()
{
    m_running = m_interface->running();
    Q_EMIT propertyChanged();

    this->animation(m_running);

    m_elapsed = m_interface->elapsed();
    Q_EMIT elapsedChanged();
}

void Timer::updateLooping()
{
    m_looping = m_interface->looping();
    Q_EMIT propertyChanged();
}

void Timer::updateCommandTimeout()
{
    m_commandTimeout = m_interface->commandTimeout();
    Q_EMIT propertyChanged();
}

void Timer::animation(bool start)
{
    if (!m_timer) {
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, [this] {
            m_elapsed = m_interface->elapsed();
            Q_EMIT this->elapsedChanged();
        });
    }

    if (start) {
        m_timer->start(250);
    } else {
        m_timer->stop();
    }
}
