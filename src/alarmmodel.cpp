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

#include "alarmmodel.h"

#include <QQmlEngine>

#include <KConfigGroup>
#include <KSharedConfig>

const QString ALARM_CFG_GROUP = "Alarms";

AlarmModel::AlarmModel(QObject *parent)
    : QAbstractListModel(parent)
{
    beginResetModel();

    // add alarms from config
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    for (QString key : group.keyList()) {
        QString json = group.readEntry(key, "");
        if (!json.isEmpty()) {
            Alarm *alarm = new Alarm(json);
            connect(alarm, &Alarm::propertyChanged, this, &AlarmModel::updateUi);

            alarmsList.append(alarm);
        }
    }

    endResetModel();

    // start alarm timer
    this->timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&AlarmModel::checkAlarmsToRun));
    timer->start(2000);
}

void AlarmModel::checkAlarmsToRun()
{
    qint64 curTime = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();

    for (Alarm *alarm : alarmsList) {
        if (!alarm || !alarm->enabled())
            continue;

        // if it is time for alarm to ring
        if (alarm->toPreviousAlarm(alarm->lastAlarm()) < alarm->toPreviousAlarm(curTime) ||                                            // is next cycle
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

    config->sync();

    emit endRemoveRows();
}

Alarm *AlarmModel::get(int index)
{
    if (index < 0 || index >= alarmsList.count())
        return new Alarm();
    return alarmsList.at(index);
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
}

void AlarmModel::updateUi()
{
    emit dataChanged(createIndex(0, 0), createIndex(alarmsList.count() - 1, 0));
}
