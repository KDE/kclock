/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
