/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "alarm.h"

#include "alarmmodel.h"
#include "settingsmodel.h"
#include "utilmodel.h"

#include <KLocalizedString>

#include <QDateTime>

Alarm::Alarm(){};
Alarm::Alarm(QString uuid)
{
    m_interface = new org::kde::kclock::Alarm(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Alarms/") + uuid, QDBusConnection::sessionBus(), this);

    if (m_interface->isValid()) {
        connect(m_interface, &OrgKdeKclockAlarmInterface::nameChanged, this, &Alarm::updateName);
        connect(m_interface, &OrgKdeKclockAlarmInterface::enabledChanged, this, &Alarm::updateEnabled);
        connect(m_interface, &OrgKdeKclockAlarmInterface::hoursChanged, this, &Alarm::updateHours);
        connect(m_interface, &OrgKdeKclockAlarmInterface::minutesChanged, this, &Alarm::updateMinutes);
        connect(m_interface, &OrgKdeKclockAlarmInterface::daysOfWeekChanged, this, &Alarm::updateDaysOfWeek);
        connect(m_interface, &OrgKdeKclockAlarmInterface::audioPathChanged, this, &Alarm::updateAudioPath);
        connect(m_interface, &OrgKdeKclockAlarmInterface::ringDurationChanged, this, &Alarm::updateRingDuration);
        connect(m_interface, &OrgKdeKclockAlarmInterface::snoozeDurationChanged, this, &Alarm::updateSnoozeDuration);
        connect(m_interface, &OrgKdeKclockAlarmInterface::snoozedLengthChanged, this, &Alarm::updateSnoozedLength);
        connect(m_interface, &OrgKdeKclockAlarmInterface::ringingChanged, this, &Alarm::updateRinging);
        connect(m_interface, &OrgKdeKclockAlarmInterface::nextRingTimeChanged, this, &Alarm::updateNextRingTime);

        m_uuid = QUuid(m_interface->uuid());
        updateName();
        updateEnabled();
        updateHours();
        updateMinutes();
        updateDaysOfWeek();
        updateAudioPath();
        updateRingDuration();
        updateSnoozeDuration();
        updateSnoozedLength();
        updateRinging();
        updateNextRingTime();

        connect(this, &Alarm::hoursChanged, this, [this] {
            Q_EMIT formattedTimeChanged();
        });
        connect(this, &Alarm::minutesChanged, this, [this] {
            Q_EMIT formattedTimeChanged();
        });
        connect(SettingsModel::instance(), &SettingsModel::timeFormatChanged, this, [this] {
            Q_EMIT formattedTimeChanged();
        });
    } else {
        m_isValid = false;
    }
}

QUuid Alarm::uuid() const
{
    return m_uuid;
}

QString Alarm::name() const
{
    return m_name;
}

void Alarm::setName(QString name)
{
    m_interface->setProperty("name", name);
}

bool Alarm::enabled() const
{
    return m_enabled;
}

void Alarm::setEnabled(bool enabled)
{
    m_interface->setProperty("enabled", enabled);
}

QString Alarm::formattedTime() const
{
    return QLocale::system().toString(QTime(m_hours, m_minutes), UtilModel::instance()->timeFormat());
}

int Alarm::hours() const
{
    return m_hours;
}

void Alarm::setHours(int hours)
{
    m_interface->setProperty("hours", hours);
}

int Alarm::minutes() const
{
    return m_minutes;
}

void Alarm::setMinutes(int minutes)
{
    m_interface->setProperty("minutes", minutes);
}

int Alarm::daysOfWeek() const
{
    return m_daysOfWeek;
}

void Alarm::setDaysOfWeek(int daysOfWeek)
{
    m_interface->setProperty("daysOfWeek", daysOfWeek);
}

QString Alarm::audioPath() const
{
    return m_audioPath;
}

void Alarm::setAudioPath(QString path)
{
    m_interface->setProperty("audioPath", path);
}

int Alarm::ringDuration() const
{
    return m_ringDuration;
}

void Alarm::setRingDuration(int ringDuration)
{
    m_interface->setProperty("ringDuration", ringDuration);
}

int Alarm::snoozeDuration() const
{
    return m_snoozeDuration;
}

void Alarm::setSnoozeDuration(int snoozeDuration)
{
    m_interface->setProperty("snoozeDuration", snoozeDuration);
}

int Alarm::snoozedLength() const
{
    return m_snoozedLength ? m_snoozedLength / 60 : 0;
}

bool Alarm::ringing() const
{
    return m_ringing;
}

quint64 Alarm::nextRingTime() const
{
    return m_nextRingTime;
}

bool Alarm::isValid() const
{
    return m_isValid;
}

void Alarm::dismiss()
{
    m_interface->dismiss();
}

void Alarm::snooze()
{
    m_interface->snooze();
}

QString Alarm::timeToRingFormatted()
{
    updateNextRingTime();
    return UtilModel::instance()->timeToRingFormatted(nextRingTime());
}

void Alarm::save()
{
    m_interface->save();
}

void Alarm::updateName()
{
    m_name = m_interface->name();
    Q_EMIT nameChanged();
}

void Alarm::updateEnabled()
{
    m_enabled = m_interface->enabled();
    Q_EMIT enabledChanged();
}

void Alarm::updateHours()
{
    m_hours = m_interface->hours();
    Q_EMIT hoursChanged();
}

void Alarm::updateMinutes()
{
    m_minutes = m_interface->minutes();
    Q_EMIT minutesChanged();
}

void Alarm::updateDaysOfWeek()
{
    m_daysOfWeek = m_interface->daysOfWeek();
    Q_EMIT daysOfWeekChanged();
}

void Alarm::updateAudioPath()
{
    m_audioPath = m_interface->audioPath();
    Q_EMIT audioPathChanged();
}

void Alarm::updateRingDuration()
{
    m_ringDuration = m_interface->ringDuration();
    Q_EMIT ringDurationChanged();
}

void Alarm::updateSnoozeDuration()
{
    m_snoozeDuration = m_interface->snoozeDuration();
    Q_EMIT snoozeDurationChanged();
}

void Alarm::updateSnoozedLength()
{
    m_snoozedLength = m_interface->snoozedLength();
    Q_EMIT snoozedLengthChanged();
}

void Alarm::updateRinging()
{
    m_ringing = m_interface->ringing();
    Q_EMIT ringingChanged();
}

void Alarm::updateNextRingTime()
{
    m_nextRingTime = m_interface->nextRingTime();
    Q_EMIT nextRingTimeChanged();
}
