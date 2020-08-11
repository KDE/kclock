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
#include <KConfigGroup>
#include <KSharedConfig>
#include <QDBusConnection>
#include <QQmlEngine>
#include <QThread>

#include "alarmmodel.h"
#include "alarms.h"
#include "alarmwaitworker.h"

#define SCRIPTANDPROPERTY QDBusConnection::ExportScriptableContents | QDBusConnection::ExportAllProperties
AlarmModel::AlarmModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // DBus
    QDBusConnection::sessionBus().registerObject("/alarms", this, QDBusConnection::ExportScriptableContents);

    beginResetModel();
    // add alarms from config
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    for (QString key : group.keyList()) {
        QString json = group.readEntry(key, "");
        if (!json.isEmpty()) {
            Alarm *alarm = new Alarm(json, this);

            alarmsList.append(alarm);
            qDebug() << QDBusConnection::sessionBus().registerObject("/alarms/" + alarm->uuid().toString(QUuid::Id128), alarm, SCRIPTANDPROPERTY);
        }
    }

    endResetModel();

    m_timerThread = new QThread(this);
    m_worker = new AlarmWaitWorker();
    m_worker->moveToThread(m_timerThread);
    connect(m_worker, &AlarmWaitWorker::finished, [this] {
        qDebug() << "ring";
        if (this->alarmToBeRung)
            alarmToBeRung->ring();
    });
    m_timerThread->start();
    scheduleAlarm();
}

quint64 AlarmModel::getNextAlarm()
{
    return nextAlarmTime;
}

void AlarmModel::scheduleAlarm()
{
    if (alarmsList.count() == 0) // no alarm, return
        return;
    qint64 minTime = std::numeric_limits<qint64>::max();
    for (auto alarm : alarmsList) {
        if (alarm->nextRingTime() > 0 && alarm->nextRingTime() < minTime) {
            alarmToBeRung = alarm;
            minTime = alarm->nextRingTime();
        }
    }
    if (minTime != std::numeric_limits<qint64>::max()) {
        qDebug() << "scheduled" << QDateTime::fromSecsSinceEpoch(minTime).toLocalTime().toString();
        nextAlarmTime = minTime;
        m_worker->setNewTime(minTime);
        emit nextAlarm(minTime);
    } else {
        qDebug() << "no alarm to ring";
        nextAlarmTime = 0;
        emit nextAlarm(0);
    }
}

/* ~ Alarm row data ~ */

QHash<int, QByteArray> AlarmModel::roleNames() const
{
    return {{HoursRole, "hours"}, {MinutesRole, "minutes"}, {NameRole, "name"}, {EnabledRole, "enabled"}, {DaysOfWeekRole, "daysOfWeek"}, {RingtonePathRole, "ringtonePath"}, {AlarmRole, "alarm"}};
}

QVariant AlarmModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= alarmsList.count()) {
        return QVariant();
    }

    auto *alarm = alarmsList[index.row()];
    if (!alarm)
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
    else if (role == AlarmRole)
        return QVariant::fromValue(alarm);
    else
        return QVariant();
}

bool AlarmModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || alarmsList.length() <= index.row())
        return false;
    // to switch or not to switch?
    auto *alarm = alarmsList[index.row()];
    if (!alarm)
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

void AlarmModel::remove(QString uuid)
{
    int index = 0;
    bool found = false;
    for (auto id : alarmsList) {
        if (id->uuid().toString() == uuid) {
            found = true;
            break;
        }
        index++;
    }
    if (!found) // do nothing if not found
        return;
    emit beginRemoveRows(QModelIndex(), index, index);

    // write to config
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    group.deleteEntry(alarmsList.at(index)->uuid().toString());
    alarmsList[index]->deleteLater(); // delete object
    alarmsList.removeAt(index);

    config->sync();

    emit endRemoveRows();
}

Alarm *AlarmModel::newAlarm()
{
    if (!tmpAlarm_) {
        tmpAlarm_ = new Alarm;
        QQmlEngine::setObjectOwnership(tmpAlarm_, QQmlEngine::CppOwnership); // prevent segfaults from js garbage collecting
    }
    return tmpAlarm_;
}
void AlarmModel::addNewAlarm()
{
    if (tmpAlarm_) {
        emit beginInsertRows(QModelIndex(), alarmsList.count(), alarmsList.count());
        alarmsList.append(tmpAlarm_);
        emit endInsertRows();
        tmpAlarm_ = nullptr;
    }

    scheduleAlarm();
    QDBusConnection::sessionBus().registerObject("/alarms/" + alarmsList.last()->uuid().toString(QUuid::Id128), alarmsList.last(), SCRIPTANDPROPERTY);
}

void AlarmModel::updateUi()
{
    emit dataChanged(createIndex(0, 0), createIndex(alarmsList.count() - 1, 0));
}

void AlarmModel::addAlarm(int hours, int minutes, int daysOfWeek, QString name, int ringTone)
{
    emit beginInsertRows(QModelIndex(), alarmsList.count(), alarmsList.count());
    alarmsList.append(new Alarm(this, name, minutes, hours, daysOfWeek));
    emit endInsertRows();
    scheduleAlarm();
    QDBusConnection::sessionBus().registerObject("/alarms/" + alarmsList.last()->uuid().toString(QUuid::Id128), alarmsList.last(), SCRIPTANDPROPERTY);
}
