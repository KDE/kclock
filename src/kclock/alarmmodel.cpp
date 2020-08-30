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
#include <KStatusNotifierItem>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QLocale>
#include <QQmlEngine>
#include <QThread>
#include <klocalizedstring.h>

#include "alarmmodel.h"
#include "alarms.h"
#include "alarmwaitworker.h"

#define SCRIPTANDPROPERTY QDBusConnection::ExportScriptableContents | QDBusConnection::ExportAllProperties
AlarmModel::AlarmModel(QObject *parent)
    : QAbstractListModel(parent)
{
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

void AlarmModel::remove(int index)
{
    if (index < 0 || index >= this->rowCount({}))
        return;

    emit beginRemoveRows(QModelIndex(), index, index);

    // write to config
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    group.deleteEntry(alarmsList.at(index)->uuid().toString());
    alarmsList[index]->deleteLater(); // delete object
    alarmsList.removeAt(index);

    // TODO: remove remote alarm

    config->sync();

    emit endRemoveRows();
}

void AlarmModel::updateUi()
{
    emit dataChanged(createIndex(0, 0), createIndex(alarmsList.count() - 1, 0));
}

Alarm *AlarmModel::addAlarm(int hours, int minutes, int daysOfWeek, QString name, QString ringtonePath)
{
    Alarm *alarm = new Alarm(this, name, minutes, hours, daysOfWeek);
    alarm->setRingtone(ringtonePath);

    // insert new alarm in order by time of day
    int i = 0;
    for (auto alarms : alarmsList) {
        if (alarms->hours() < hours) {
            i++;
            continue;
        } else if (alarms->hours() == hours) {
            if (alarms->minutes() < minutes) {
                i++;
                continue;
            } else {
                break;
            }
        } else {
            break;
        }
    }
    emit beginInsertRows(QModelIndex(), i, i);
    alarmsList.insert(i, alarm);
    emit endInsertRows();

    // TODO: add alarm to remote
    return alarm;
}
