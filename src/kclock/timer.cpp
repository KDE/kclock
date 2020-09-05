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
#include <QTimer>

#include "timer.h"
/* ~ Timer ~ */
Timer::Timer()
{
}
Timer::Timer(QString uuid, bool justCreated)
    : m_interface(new OrgKdeKclockTimerInterface(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Timers/") + uuid, QDBusConnection::sessionBus(), this))
    , m_justCreated(justCreated)
{
    if (m_interface->isValid()) {
        m_uuid = QUuid(m_interface->getUUID());
        m_label = m_interface->label();
        m_length = m_interface->length();
        m_running = m_interface->running();
        m_elapsed = m_interface->elapsed();
        connect(m_interface, &OrgKdeKclockTimerInterface::lengthChanged, this, &Timer::updateLength);
        connect(m_interface, &OrgKdeKclockTimerInterface::labelChanged, this, &Timer::updateLabel);
        connect(m_interface, &OrgKdeKclockTimerInterface::runningChanged, this, &Timer::updateRunning);

        updateRunning(); // start animation
    }
}

void Timer::toggleRunning()
{
    m_interface->toggleRunning();
}

void Timer::reset()
{
    m_interface->reset();

    m_elapsed = m_interface->elapsed();
    Q_EMIT elapsedChanged();
};

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
