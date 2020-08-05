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
#include <QMediaPlayer>
#include <QQmlEngine>
#include <QTime>
#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KNotification>
#include <KSharedConfig>

#include "alarms.h"
#include "kclocksettings.h"

const QString ALARM_CFG_GROUP = "Alarms";

// alarm created from UI
Alarm::Alarm(QObject *parent, QString name, int minutes, int hours, int daysOfWeek)
    : QObject(parent)
{
    enabled_ = true;
    uuid_ = QUuid::createUuid();
    name_ = name;
    minutes_ = minutes;
    hours_ = hours;
    daysOfWeek_ = daysOfWeek;
    lastAlarm_ = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();

    ringtonePlayer = new QMediaPlayer(this, QMediaPlayer::LowLatency);
    ringtonePlayer->setVolume(volume_);
    connect(ringtonePlayer, &QMediaPlayer::stateChanged, this, &Alarm::loopAlarmSound);

    ringtonePlayer->setMedia(audioPath_);
}

// alarm from json (loaded from storage)
Alarm::Alarm(QString serialized)
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
        lastAlarm_ = obj["lastAlarm"].toInt();
        snooze_ = obj["snooze"].toInt();
        lastSnooze_ = obj["lastSnooze"].toInt();
        ringtoneName_ = obj["ringtoneName"].toString();
        audioPath_ = QUrl::fromLocalFile(obj["audioPath"].toString());
        volume_ = obj["volume"].toInt();
    }

    ringtonePlayer = new QMediaPlayer(this, QMediaPlayer::LowLatency);
    ringtonePlayer->setVolume(volume_);
    connect(ringtonePlayer, &QMediaPlayer::stateChanged, this, &Alarm::loopAlarmSound);

    ringtonePlayer->setMedia(audioPath_);
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
    obj["lastAlarm"] = lastAlarm();
    obj["snooze"] = snooze();
    obj["lastSnooze"] = lastSnooze();
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
    qDebug("Found alarm to run, sending notification...");

    KNotification *notif = new KNotification("timerFinished");
    notif->setActions(QStringList() << "Dismiss"
                                    << "Snooze");
    notif->setIconName("kclock");
    notif->setTitle(name());
    notif->setText(QDateTime::currentDateTime().toLocalTime().toString("hh:mm ap")); // TODO
    notif->setDefaultAction(i18n("View"));
    notif->setUrgency(KNotification::HighUrgency);
    notif->setFlags(KNotification::NotificationFlag::LoopSound | KNotification::NotificationFlag::Persistent);

    connect(notif, &KNotification::defaultActivated, this, &Alarm::handleDismiss);
    connect(notif, &KNotification::action1Activated, this, &Alarm::handleDismiss);
    connect(notif, &KNotification::action2Activated, this, &Alarm::handleSnooze);
    connect(notif, &KNotification::closed, this, &Alarm::handleDismiss);

    notif->sendEvent();

    alarmNotifOpen = true;
    alarmNotifOpenTime = QTime::currentTime();
    // play sound (it will loop)
    qDebug() << "Alarm sound: " << audioPath_;
    ringtonePlayer->play();
}

void Alarm::loopAlarmSound(QMediaPlayer::State state)
{
    KClockSettings settings;
    if (state == QMediaPlayer::StoppedState && alarmNotifOpen && (alarmNotifOpenTime.secsTo(QTime::currentTime()) <= settings.alarmSilenceAfter())) {
        ringtonePlayer->play();
    }
}

void Alarm::handleDismiss()
{
    alarmNotifOpen = false;

    qDebug() << "Alarm dismissed";
    ringtonePlayer->stop();

    setLastSnooze(0);
    save();
}

void Alarm::handleSnooze()
{
    KClockSettings settings;
    alarmNotifOpen = false;
    qDebug() << "Alarm snoozed (" << settings.alarmSnoozeLengthDisplay() << ")" << lastSnooze();
    ringtonePlayer->stop();

    setSnooze(lastSnooze() + 60 * settings.alarmSnoozeLength()); // snooze 5 minutes
    setLastSnooze(snooze());
    setEnabled(true);
    save();

    emit propertyChanged();
}

qint64 Alarm::toPreviousAlarm(qint64 timestamp)
{
    QDateTime date = QDateTime::fromSecsSinceEpoch(timestamp).toLocalTime(); // local time
    QTime alarmTime = QTime(hours(), minutes());

    if (daysOfWeek() == 0) {            // no repeat of alarm
        if (alarmTime <= date.time()) { // current day
            return QDateTime(date.date(), alarmTime).toSecsSinceEpoch();
        } else { // previous day
            return QDateTime(date.addDays(-1).date(), alarmTime).toSecsSinceEpoch();
        }
    } else { // repeat alarm
        bool first = true;

        // keeping looping back a single day until the day of week is accepted
        while (((daysOfWeek() & (1 << (date.date().dayOfWeek() - 1))) == 0) // check day
               || (first && (alarmTime > date.time())))                     // check time on first day
        {
            date = date.addDays(-1); // go back a day
            first = false;
        }

        return QDateTime(date.date(), alarmTime).toSecsSinceEpoch();
    }
}

Alarm::~Alarm()
{
    delete ringtonePlayer;
}
