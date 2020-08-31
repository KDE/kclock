/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *                Han Young <hanyoung@protonmail.com>
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

#include <QDateTime>
#include <QDebug>

#include <KLocalizedString>

#include "alarmmodel.h"
#include "alarms.h"
Alarm::Alarm() {};
Alarm::Alarm(QString uuid)
    : m_uuid(uuid)
{
    m_interface = new org::kde::kclock::Alarm(QStringLiteral("org.kde.kclockd"), QStringLiteral("/alarms/") + uuid, QDBusConnection::sessionBus(), this);

    if (m_interface->isValid()) {
        connect(m_interface, SIGNAL(propertyChanged(QString)), this, SLOT(updateProperty(QString)));

        m_name = m_interface->name();
        m_enabled = m_interface->enabled();
        m_hours = m_interface->hours();
        m_minutes = m_interface->minutes();
        m_daysOfWeek = m_interface->daysOfWeek();
        m_snooze = m_interface->snoozedMinutes() * 60;
        m_ringtonePath = m_interface->ringtonePath();
        m_nextRingTime = m_interface->nextRingTime();
    } else {
        m_isValid = false;
    }
}

QString Alarm::timeToRingFormated() const
{
    auto remaining = this->nextRingTime() - QDateTime::currentSecsSinceEpoch();
    int day = remaining / (24 * 3600);
    int hour = remaining / 3600 - day * 24;
    int minute = remaining / 60 - day * 24 * 60 - hour * 60;
    QString arg;
    if (day > 0) {
        arg += i18np("%1 day", "%1 days", day);
    }
    if (hour > 0) {
        if (day > 0 && minute > 0) {
            arg += i18n(", ");
        } else if (day > 0) {
            arg += i18n(" and ");
        }
        arg += i18np("%1 hour", "%1 hours", hour);
    }
    if (minute > 0) {
        if (day > 0 || hour > 0) {
            arg += i18n(" and ");
        }
        arg += i18np("%1 minute", "%1 minutes", minute);
    }
    return i18n("Alarm will be rung after %1", arg);
}

void Alarm::updateProperty(QString property)
{
    this->setProperty(property.toUtf8(), m_interface->property(property.toUtf8()));
}
