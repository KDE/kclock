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

#include "alarm.h"

#include "alarmmodel.h"
#include "utilmodel.h"

#include <KLocalizedString>

#include <QDateTime>

Alarm::Alarm(){};
Alarm::Alarm(QString uuid)
{
    m_interface = new org::kde::kclock::Alarm(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Alarms/") + uuid, QDBusConnection::sessionBus(), this);

    if (m_interface->isValid()) {
        connect(m_interface, &OrgKdeKclockAlarmInterface::propertyChanged, this, &Alarm::updateProperty);
        connect(m_interface, &OrgKdeKclockAlarmInterface::alarmChanged, [this] {
            m_nextRingTime = m_interface->nextRingTime();
        });

        m_uuid = QUuid(m_interface->getUUID());
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

QString Alarm::timeToRingFormated()
{
    this->calculateNextRingTime();

    return UtilModel::instance()->timeToRingFormatted(this->nextRingTime());
}

void Alarm::updateProperty(QString property)
{
    if (property == QStringLiteral("name"))
        this->m_name = m_interface->property("name").toString();
    else if (property == QStringLiteral("enabled"))
        this->m_enabled = m_interface->property("enabled").toBool();
    else if (property == QStringLiteral("hours"))
        this->m_hours = m_interface->property("hours").toInt();
    else if (property == QStringLiteral("minutes"))
        this->m_minutes = m_interface->property("minutes").toInt();
    else if (property == QStringLiteral("daysOfWeek"))
        this->m_daysOfWeek = m_interface->property("daysOfWeek").toInt();
    else if (property == QStringLiteral("snoozedMinutes"))
        this->m_snooze = m_interface->property("snoozedMinutes").toInt() * 60;
    else if (property == QStringLiteral("ringtonePath"))
        this->m_ringtonePath = m_interface->property("ringtonePath").toBool();
    Q_EMIT propertyChanged();
}

void Alarm::calculateNextRingTime()
{
    if (!this->m_enabled) { // if not enabled, means this would never ring
        m_nextRingTime = -1;
        return;
    }

    m_nextRingTime = UtilModel::instance()->calculateNextRingTime(this->m_hours, this->m_minutes, this->m_daysOfWeek, this->m_snooze);
}
