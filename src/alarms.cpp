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

#include <QDBusConnection>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTime>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KNotification>
#include <KSharedConfig>

#include "alarmadaptor.h"
#include "alarmmodel.h"
#include "alarmplayer.h"
#include "alarms.h"
#include "alarmwaitworker.h"
#include "kclockdsettings.h"

// alarm created from UI
Alarm::Alarm(AlarmModel *parent, QString name, int minutes, int hours, int daysOfWeek)
    : QObject(parent)
    , m_uuid(QUuid::createUuid())
    , m_enabled(true)
    , m_name(name)
    , m_minutes(minutes)
    , m_hours(hours)
    , m_daysOfWeek(daysOfWeek)
{
    initialize(parent);
}

// alarm from json (loaded from storage)
Alarm::Alarm(QString serialized, AlarmModel *parent)
    : QObject(parent)
{
    if (serialized.isEmpty()) {
        m_uuid = QUuid::createUuid();
    } else {
        QJsonDocument doc = QJsonDocument::fromJson(serialized.toUtf8());
        QJsonObject obj = doc.object();

        m_uuid = QUuid::fromString(obj[QStringLiteral("uuid")].toString());
        m_name = obj[QStringLiteral("name")].toString();
        m_minutes = obj[QStringLiteral("minutes")].toInt();
        m_hours = obj[QStringLiteral("hours")].toInt();
        m_daysOfWeek = obj[QStringLiteral("daysOfWeek")].toInt();
        m_enabled = obj[QStringLiteral("enabled")].toBool();
        m_snooze = obj[QStringLiteral("snooze")].toInt();
        m_audioPath = QUrl::fromLocalFile(obj[QStringLiteral("audioPath")].toString());
    }
    initialize(parent);
}

void Alarm::initialize(AlarmModel *parent)
{
    connect(this, &Alarm::alarmChanged, this, &Alarm::save);
    connect(this, &Alarm::alarmChanged, this, &Alarm::calculateNextRingTime); // the slots will be called according to the order they have been connected.
                                                                              // always connect this before calling AlarmModel::scheduleAlarm

    calculateNextRingTime();

    if (parent) {
        connect(this, &Alarm::alarmChanged, parent, &AlarmModel::scheduleAlarm); // connect this last
    }

    // DBus
    new AlarmAdaptor(this);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/alarms/") + this->uuid().toString(QUuid::Id128), this);
    connect(this, &QObject::destroyed, [this] { QDBusConnection::sessionBus().unregisterObject(QStringLiteral("/alarms/") + this->uuid().toString(QUuid::Id128), QDBusConnection::UnregisterNode); });
}

// alarm to json
QString Alarm::serialize()
{
    QJsonObject obj;
    obj[QStringLiteral("uuid")] = uuid().toString();
    obj[QStringLiteral("name")] = name();
    obj[QStringLiteral("minutes")] = minutes();
    obj[QStringLiteral("hours")] = hours();
    obj[QStringLiteral("daysOfWeek")] = daysOfWeek();
    obj[QStringLiteral("enabled")] = enabled();
    obj[QStringLiteral("snooze")] = snooze();
    obj[QStringLiteral("audioPath")] = m_audioPath.toLocalFile();
    return QString(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

void Alarm::save()
{
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    group.writeEntry(uuid().toString(), this->serialize());
    group.sync();
}

void Alarm::ring()
{
    // if not enabled, don't ring
    if (!this->enabled())
        return;

    qDebug() << "Ringing alarm" << m_name << "and sending notification...";

    KNotification *notif = new KNotification(QStringLiteral("alarm"));
    notif->setActions(QStringList {i18n("Dismiss"), i18n("Snooze")});
    notif->setIconName(QStringLiteral("kclock"));
    notif->setTitle(name());
    notif->setText(QLocale::system().toString(QTime::currentTime(), QLocale::ShortFormat)); // TODO
    notif->setDefaultAction(i18n("View"));
    notif->setFlags(KNotification::NotificationFlag::Persistent);

    connect(notif, &KNotification::defaultActivated, this, &Alarm::handleDismiss);
    connect(notif, &KNotification::action1Activated, this, &Alarm::handleDismiss);
    connect(notif, &KNotification::action2Activated, this, &Alarm::handleSnooze);
    connect(notif, &KNotification::closed, this, &Alarm::handleDismiss);
    connect(notif, &KNotification::closed, [notif] { notif->close(); });

    notif->sendEvent();

    alarmNotifOpen = true;
    alarmNotifOpenTime = QTime::currentTime();

    // play sound (it will loop)
    qDebug() << "Alarm sound: " << m_audioPath;
    AlarmPlayer::instance().setSource(this->m_audioPath);
    AlarmPlayer::instance().play();
}

void Alarm::handleDismiss()
{
    alarmNotifOpen = false;

    qDebug() << "Alarm" << m_name << "dismissed";
    AlarmPlayer::instance().stop();

    // ignore if the snooze button was pressed and dismiss is still called
    if (!m_justSnoozed) {
        // disable alarm if set to run once
        if (daysOfWeek() == 0) {
            setEnabled(false);
        }
    } else {
        qDebug() << "Ignore dismiss (triggered by snooze)" << m_snooze;
    }

    m_justSnoozed = false;

    save();
    Q_EMIT alarmChanged();
}

void Alarm::handleSnooze()
{
    m_justSnoozed = true;

    alarmNotifOpen = false;
    qDebug() << "Alarm snoozed (" << KClockSettings::self()->alarmSnoozeLength() << ")";
    AlarmPlayer::instance().stop();

    setSnooze(snooze() + 60 * KClockSettings::self()->alarmSnoozeLength()); // snooze 5 minutes
    m_enabled = true;                                                       // can't use setSnooze because it resets snooze time
    save();
    Q_EMIT alarmChanged();
}

void Alarm::calculateNextRingTime()
{
    if (!this->m_enabled) { // if not enabled, means this would never ring
        m_nextRingTime = -1;
        return;
    }

    // get the time that the alarm will ring on the day
    QTime alarmTime = QTime(this->m_hours, this->m_minutes, 0).addSecs(this->m_snooze);

    QDateTime date = QDateTime::currentDateTime();

    if (this->m_daysOfWeek == 0) {      // alarm does not repeat (no days of the week are specified)
        if (alarmTime >= date.time()) { // alarm occurs later today
            m_nextRingTime = QDateTime(date.date(), alarmTime).toSecsSinceEpoch();
        } else { // alarm occurs on the next day
            m_nextRingTime = QDateTime(date.date().addDays(1), alarmTime).toSecsSinceEpoch();
        }
    } else { // repeat alarm
        bool first = true;

        // keeping looping forward a single day until the day of week is accepted
        while (((this->m_daysOfWeek & (1 << (date.date().dayOfWeek() - 1))) == 0) // check day
               || (first && (alarmTime < date.time())))                           // check time if the current day is accepted (keep looping forward if alarmTime has passed)
        {
            date = date.addDays(1); // go forward a day
            first = false;
        }

        m_nextRingTime = QDateTime(date.date(), alarmTime).toSecsSinceEpoch();
    }
}

qint64 Alarm::nextRingTime()
{
    // day changed, re-calculate
    if (this->m_nextRingTime < QDateTime::currentSecsSinceEpoch()) {
        calculateNextRingTime();
    }
    return m_nextRingTime;
}
