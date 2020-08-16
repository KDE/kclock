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
#include <QJsonDocument>
#include <QJsonObject>
#include <QTime>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KNotification>
#include <KSharedConfig>

#include "alarmmodel.h"
#include "alarmplayer.h"
#include "alarms.h"
#include "alarmwaitworker.h"
#include "kclocksettings.h"

// alarm created from UI
Alarm::Alarm(AlarmModel *parent, QString name, int minutes, int hours, int daysOfWeek)
    : QObject(parent)
    , uuid_(QUuid::createUuid())
    , enabled_(true)
    , name_(name)
    , minutes_(minutes)
    , hours_(hours)
    , daysOfWeek_(daysOfWeek)
{
    connect(this, &Alarm::alarmChanged, this, &Alarm::save);
    connect(this, &Alarm::alarmChanged, this, &Alarm::calculateNextRingTime); // the slots will be called according to
                                                                              // the order they have been connected
                                                                              // always connect this first than AlarmModel::scheduleAlarm

    calculateNextRingTime();

    if (parent) {
        connect(this, &Alarm::propertyChanged, parent, &AlarmModel::updateUi);
        connect(this, &Alarm::alarmChanged, parent, &AlarmModel::scheduleAlarm); // connect this last
    }
}

// alarm from json (loaded from storage)
Alarm::Alarm(QString serialized, AlarmModel *parent)
    : QObject(parent)
{
    if (serialized == "") {
        uuid_ = QUuid::createUuid();
    } else {
        QJsonDocument doc = QJsonDocument::fromJson(serialized.toUtf8());
        QJsonObject obj = doc.object();

        uuid_ = QUuid::fromString(obj["uuid"].toString());
        name_ = obj["name"].toString();
        minutes_ = obj["minutes"].toInt();
        hours_ = obj["hours"].toInt();
        daysOfWeek_ = obj["daysOfWeek"].toInt();
        enabled_ = obj["enabled"].toBool();
        snooze_ = obj["snooze"].toInt();
        ringtoneName_ = obj["ringtoneName"].toString();
        audioPath_ = QUrl::fromLocalFile(obj["audioPath"].toString());
        volume_ = obj["volume"].toInt();
    }

    connect(this, &Alarm::alarmChanged, this, &Alarm::save);
    connect(this, &Alarm::alarmChanged, this, &Alarm::calculateNextRingTime); // the slots will be called according to
                                                                              // the order they have been connected
                                                                              // always connect this first than AlarmModel::scheduleAlarm

    calculateNextRingTime();

    if (parent) {
        connect(this, &Alarm::propertyChanged, parent, &AlarmModel::updateUi);
        connect(this, &Alarm::alarmChanged, parent, &AlarmModel::scheduleAlarm); // connect this last
    }
}

// alarm to json
QString Alarm::serialize()
{
    QJsonObject obj;
    obj["uuid"] = uuid().toString();
    obj["name"] = name();
    obj["minutes"] = minutes();
    obj["hours"] = hours();
    obj["daysOfWeek"] = daysOfWeek();
    obj["enabled"] = enabled();
    obj["snooze"] = snooze();
    obj["ringtoneName"] = ringtoneName();
    obj["audioPath"] = audioPath_.toLocalFile();
    obj["volume"] = volume_;
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

    qDebug("Found alarm to run, sending notification...");

    KNotification *notif = new KNotification("timerFinished");
    notif->setActions(QStringList() << "Dismiss"
                                    << "Snooze");
    notif->setIconName("kclock");
    notif->setTitle(name());
    notif->setText(QDateTime::currentDateTime().toLocalTime().toString("hh:mm ap")); // TODO
    notif->setDefaultAction(i18n("View"));
    notif->setUrgency(KNotification::HighUrgency);
    notif->setFlags(KNotification::NotificationFlag::Persistent);

    connect(notif, &KNotification::defaultActivated, this, &Alarm::handleDismiss);
    connect(notif, &KNotification::action1Activated, this, &Alarm::handleDismiss);
    connect(notif, &KNotification::action2Activated, this, &Alarm::handleSnooze);
    connect(notif, &KNotification::closed, this, &Alarm::handleDismiss);

    notif->sendEvent();

    alarmNotifOpen = true;
    alarmNotifOpenTime = QTime::currentTime();
    
    // play sound (it will loop)
    qDebug() << "Alarm sound: " << audioPath_;
    AlarmPlayer::instance().setSource(this->audioPath_);
    AlarmPlayer::instance().setVolume(this->volume_);
    AlarmPlayer::instance().play();
}

void Alarm::handleDismiss()
{
    alarmNotifOpen = false;

    qDebug() << "Alarm dismissed";
    AlarmPlayer::instance().stop();

    // ignore if the snooze button was pressed and dismiss is still called
    if (!m_justSnoozed) {
        // disable alarm if set to run once
        if (daysOfWeek() == 0) {
            setEnabled(false);
        }
    } else {
        qDebug() << "Ignore dismiss (triggered by snooze)" << snooze_;
    }

    m_justSnoozed = false;
    
    save();
    emit alarmChanged();
}

void Alarm::handleSnooze()
{
    m_justSnoozed = true;
    
    KClockSettings settings;
    alarmNotifOpen = false;
    qDebug() << "Alarm snoozed (" << settings.alarmSnoozeLengthDisplay() << ")";
    AlarmPlayer::instance().stop();

    setSnooze(snooze() + 60 * settings.alarmSnoozeLength()); // snooze 5 minutes
    enabled_ = true; // can't use setSnooze because it resets snooze time
    save();
    
    emit propertyChanged();
    emit alarmChanged();
}

void Alarm::calculateNextRingTime()
{
    if (!this->enabled_) { // if not enabled, means this would never ring
        m_nextRingTime = -1;
        return;
    }
    
    // get the time that the alarm will ring on the day
    QTime alarmTime = QTime(this->hours_, this->minutes_, 0).addSecs(this->snooze_);
    
    QDateTime date = QDateTime::currentDateTime();

    if (this->daysOfWeek_ == 0) { // alarm does not repeat (no days of the week are specified)
        if (alarmTime >= date.time()) { // alarm occurs later today
            m_nextRingTime = QDateTime(date.date(), alarmTime).toSecsSinceEpoch();
        } else { // alarm occurs on the next day
            m_nextRingTime = QDateTime(date.date().addDays(1), alarmTime).toSecsSinceEpoch();
        }
    } else { // repeat alarm
        bool first = true;

        // keeping looping forward a single day until the day of week is accepted
        while (((this->daysOfWeek_ & (1 << (date.date().dayOfWeek() - 1))) == 0) // check day
               || (first && (alarmTime < date.time())))                          // check time if the current day is accepted (keep looping forward if alarmTime has passed)
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
