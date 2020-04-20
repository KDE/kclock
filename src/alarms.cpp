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

#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

#include <KSharedConfig>
#include <KConfigGroup>

#include "alarms.h"

const QString ALARM_CFG_GROUP = "Alarms";

Alarm::Alarm(QObject *parent, QString name, int minutes, int hours, int dayOfWeek) 
{
    enabled = true;
    uuid = QUuid::createUuid();
    this->name = name;
    this->minutes = minutes;
    this->hours = hours;
    this->dayOfWeek = dayOfWeek;
}

// alarm from json
Alarm::Alarm(QString serialized)
{
    if (serialized == "") {
        uuid = QUuid::createUuid();
    } else {
        QJsonDocument doc = QJsonDocument::fromJson(serialized.toUtf8());
        QJsonObject obj = doc.object();
        uuid = QUuid::fromString(obj["uuid"].toString());
        name = obj["name"].toString();
        minutes = obj["minutes"].toInt();
        hours = obj["hours"].toInt();
        dayOfWeek = obj["dayOfWeek"].toInt();
        enabled = obj["enabled"].toBool();
    }
}

// alarm to json
QString Alarm::serialize()
{
    QJsonObject obj;
    obj["uuid"] = this->uuid.toString();
    obj["name"] = this->name;
    obj["minutes"] = this->minutes;
    obj["hours"] = this->hours;
    obj["dayOfWeek"] = this->dayOfWeek;
    obj["enabled"] = this->enabled;
    return QString(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

AlarmModel::AlarmModel(QObject *parent)
{
    // add alarms from config
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    for (QString key : group.keyList()) {
        QString json = group.readEntry(key, "");
        if (json != "") 
            alarmsList.append(new Alarm(json));
    }
}

/* ~ Alarm row data ~ */

QHash<int, QByteArray> AlarmModel::roleNames() const {
    return {
            {HoursRole, "hours"},
            {MinutesRole, "minutes"},
            {NameRole, "name"},
            {EnabledRole, "enabled"},
            {DayOfWeekRole, "dayOfWeek"},
    };
}

QVariant AlarmModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= alarmsList.count()) return QVariant();

    auto* alarm = alarmsList[index.row()];
    if (role == EnabledRole) return alarm->isEnabled();
    else if (role == HoursRole) return alarm->getHours();
    else if (role == MinutesRole) return alarm->getMinutes();
    else if (role == NameRole) return alarm->getName();
    else if (role == DayOfWeekRole) return alarm->getDayOfWeek();
    else return QVariant();
}

bool AlarmModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || alarmsList.length() <= index.row()) return false;

    auto* alarm = alarmsList[index.row()];
    if (role == EnabledRole) alarm->setEnabled(value.toBool());
    else if (role == HoursRole) alarm->setHours(value.toInt());
    else if (role == MinutesRole) alarm->setMinutes(value.toInt());
    else if (role == NameRole) alarm->setName(value.toString());
    else if (role == DayOfWeekRole) alarm->setDayOfWeek(value.toInt());
    else return false; 

    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    group.writeEntry(alarm->getUuid().toString(), alarm->serialize());
    
    emit dataChanged(index, index);
    return true;
}

int AlarmModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return alarmsList.size();
}

Qt::ItemFlags AlarmModel::flags(const QModelIndex& index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEditable;
}

Alarm* AlarmModel::insert(int index, QString name, int minutes, int hours, int dayOfWeek)
{
    if (index < 0 || index > alarmsList.count()) return new Alarm();
    emit beginInsertRows(QModelIndex(), index, index);
    
    auto* alarm = new Alarm(this, name, minutes, hours, dayOfWeek);
    alarmsList.insert(index, alarm);
    
    // write to config
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    group.writeEntry(alarm->getUuid().toString(), alarm->serialize());
    
    emit endInsertRows();
    return alarm;
}

void AlarmModel::remove(int index)
{
    if (index < 0 || index >= alarmsList.count()) return;
    emit beginRemoveRows(QModelIndex(), index, index);
    
    // write to config
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    group.deleteEntry(alarmsList.at(index)->getUuid().toString());
    
    alarmsList.removeAt(index);
    
    emit endRemoveRows();
}

Alarm* AlarmModel::get(int index)
{
    if (index < 0 || index >= alarmsList.count()) return new Alarm();
    return alarmsList.at(index);
}

void AlarmModel::updateUi()
{
    emit dataChanged(createIndex(0, 0), createIndex(alarmsList.count()-1, 0));
}

