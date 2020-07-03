/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
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
#include "settingsmodel.h"

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
    ringtonePlayer->setVolume(100);
    connect(ringtonePlayer, &QMediaPlayer::stateChanged, this, &Alarm::loopAlarmSound);
    
    ringtonePlayer->setMedia(audioPath);
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
        audioPath = QUrl::fromLocalFile(obj["audioPath"].toString());
    }
    
    ringtonePlayer = new QMediaPlayer(this, QMediaPlayer::LowLatency);
    ringtonePlayer->setVolume(100);
    connect(ringtonePlayer, &QMediaPlayer::stateChanged, this, &Alarm::loopAlarmSound);
    
    ringtonePlayer->setMedia(audioPath);
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
    obj["audioPath"] = audioPath.toLocalFile();
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
    notif->setActions(QStringList() << "Dismiss" << "Snooze");
    notif->setIconName("kclock");
    notif->setTitle(name());
    notif->setText(QDateTime::currentDateTime().toLocalTime().toString("hh:mm ap")); // TODO
    notif->setDefaultAction(i18n("View"));
    notif->setUrgency(KNotification::HighUrgency);
    notif->setFlags(KNotification::NotificationFlag::LoopSound | KNotification::NotificationFlag::Persistent);

    connect(notif, &KNotification::defaultActivated, this, &Alarm::handleDismiss);
    connect(notif, &KNotification::action1Activated, this, &Alarm::handleDismiss);
    connect(notif, &KNotification::action2Activated, this, &Alarm::handleSnooze);

    notif->sendEvent();
    
    alarmNotifOpen = true;
    alarmNotifOpenTime = QTime::currentTime();
    // play sound (it will loop)
    qDebug() << "Alarm sound: " << audioPath;
    ringtonePlayer->play();
}

void Alarm::loopAlarmSound(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState && alarmNotifOpen && (alarmNotifOpenTime.secsTo(QTime::currentTime()) <= SettingsModel::inst()->alarmSilenceAfter())) {
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
    alarmNotifOpen = false;
    qDebug() << "Alarm snoozed (" << SettingsModel::inst()->alarmSnoozeLengthDisplay() << ")" << lastSnooze();
    ringtonePlayer->stop();
    
    setSnooze(lastSnooze() + 60 * SettingsModel::inst()->alarmSnoozeLength()); // snooze 5 minutes
    setLastSnooze(snooze());
    setEnabled(true);
    save();
    
    emit onPropertyChanged();
}

qint64 Alarm::toPreviousAlarm(qint64 timestamp)
{
    QDateTime date = QDateTime::fromSecsSinceEpoch(timestamp).toLocalTime(); // local time
    QTime alarmTime = QTime(hours(), minutes());

    if (daysOfWeek() == 0) { // no repeat of alarm
        if (alarmTime <= date.time()) { // current day
            return QDateTime(date.date(), alarmTime).toSecsSinceEpoch();
        } else { // previous day
            return QDateTime(date.addDays(-1).date(), alarmTime).toSecsSinceEpoch();
        }
    } else { // repeat alarm
        bool first = true;

        // keeping looping back a single day until the day of week is accepted
        while (((daysOfWeek() & (1 << (date.date().dayOfWeek() - 1))) == 0) // check day
               || (first && (alarmTime > date.time())))                             // check time on first day
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

/* ~~~ Alarm Model ~~~ */

AlarmModel::AlarmModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // add alarms from config
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    for (QString key : group.keyList()) {
        QString json = group.readEntry(key, "");
        if (json != "")
            alarmsList.append(new Alarm(json));
    }

    // start alarm timer
    this->timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&AlarmModel::checkAlarmsToRun));
    timer->start(2000);
}

void AlarmModel::checkAlarmsToRun()
{
    qint64 curTime = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();

    for (Alarm *alarm : alarmsList) {
        if (alarm == nullptr || !alarm->enabled())
            continue;

        // if it is time for alarm to ring
        if (alarm->toPreviousAlarm(alarm->lastAlarm()) < alarm->toPreviousAlarm(curTime) ||                                                  // is next cycle
            (alarm->snooze() != 0 && alarm->toPreviousAlarm(alarm->lastAlarm()) == alarm->toPreviousAlarm(curTime - alarm->snooze()))) // snooze
        {
            // ring alarm and set last time the alarm rang
            if (60 * 5 > (curTime - alarm->toPreviousAlarm(curTime) - alarm->snooze())) // only ring if it has been within 5 minutes of the alarm time
                alarm->ring();
            
            // reset snooze (stored in lastSnooze if the snooze button is clicked)
            alarm->setSnooze(0);
            alarm->setLastAlarm(curTime);

            // disable alarm if run once
            if (alarm->daysOfWeek() == 0)
                alarm->setEnabled(false);

            // save alarm after run
            alarm->save();
            updateUi();
        }
    }
}

/* ~ Alarm row data ~ */

QHash<int, QByteArray> AlarmModel::roleNames() const
{
    return {{HoursRole, "hours"},
            {MinutesRole, "minutes"},
            {NameRole, "name"},
            {EnabledRole, "enabled"},
            {DaysOfWeekRole, "daysOfWeek"},
            {RingtonePathRole, "ringtonePath"}};
}

QVariant AlarmModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= alarmsList.count())
        return QVariant();

    auto *alarm = alarmsList[index.row()];
    if (alarm == nullptr)
        return false;
    if (role == EnabledRole)
        return alarm->enabled();
    else if (role == HoursRole)
        return alarm->hours();
    else if (role == MinutesRole)
        return alarm->minutes();
    else if (role == NameRole)
        return alarm->name();
    else if (role == DaysOfWeekRole)
        return alarm->daysOfWeek();
    else
        return QVariant();
}

bool AlarmModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || alarmsList.length() <= index.row())
        return false;
    // to switch or not to switch?
    auto *alarm = alarmsList[index.row()];
    if (alarm == nullptr)
        return false;
    if (role == EnabledRole)
        alarm->setEnabled(value.toBool());
    else if (role == HoursRole)
        alarm->setHours(value.toInt());
    else if (role == MinutesRole)
        alarm->setMinutes(value.toInt());
    else if (role == NameRole)
        alarm->setName(value.toString());
    else if (role == DaysOfWeekRole)
        alarm->setDaysOfWeek(value.toInt());
    else if (role == RingtonePathRole)
        alarm->setRingtone(value.toString());
    else
        return false;

    alarm->save();

    emit dataChanged(index, index);
    return true;
}

int AlarmModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return alarmsList.size();
}

Qt::ItemFlags AlarmModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEditable;
}

void AlarmModel::newAlarm(QString name, int minutes, int hours, int daysOfWeek, QUrl ringtone)
{
    auto index = alarmsList.count();
    auto alarm = new Alarm(this, name, minutes, hours, daysOfWeek);
    
    if (ringtone.isValid())
        alarm->setRingtone(ringtone.toLocalFile());
    
    QQmlEngine::setObjectOwnership(alarm, QQmlEngine::CppOwnership); // prevent segfaults from js garbage collecting
    
    emit beginInsertRows(QModelIndex(), index, index);
    alarmsList.insert(index, alarm);
    
    // write to config
    alarm->save();
    
    emit endInsertRows();
}

void AlarmModel::remove(int index)
{
    if (index < 0 || index >= alarmsList.count())
        return;
    emit beginRemoveRows(QModelIndex(), index, index);

    // write to config
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    group.deleteEntry(alarmsList.at(index)->uuid().toString());
    alarmsList[index]->deleteLater(); // delete object
    alarmsList.removeAt(index);

    emit endRemoveRows();
}

Alarm *AlarmModel::get(int index)
{
    if (index < 0 || index >= alarmsList.count())
        return new Alarm();
    return alarmsList.at(index);
}

void AlarmModel::updateUi()
{
    emit dataChanged(createIndex(0, 0), createIndex(alarmsList.count() - 1, 0));
}
