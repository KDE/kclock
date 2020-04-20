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
#include <sstream>
#include <QtCore/QProcess>
#include <QDebug>
#include "alarms.h"

Alarm::Alarm(QObject *parent, QString name, int minutes, int hours, int dayOfWeek) {
    enabled = true;
    uuid = QUuid::createUuid();
    this->name = name;
    this->cronMinutes = minutes;
    this->cronHours = hours;
    this->cronDayOfWeek = dayOfWeek;
}

QString Alarm::getCronString()
{
    std::string s = std::to_string(getMinutes()) + " " + std::to_string(getHours()) + " * * * ";
    return QString::fromStdString(s).append(getDayOfWeek())
            .arg(QCoreApplication::applicationFilePath(), uuid.toString());
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
    emit endInsertRows();
    return alarm;
}

void AlarmModel::remove(int index)
{
    if (index < 0 || index >= alarmsList.count()) return;
    emit beginRemoveRows(QModelIndex(), index, index);
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

bool AlarmModel::load()
{
    QProcess crontab;
    crontab.setProgram("crontab");
    crontab.setArguments(QStringList() << "-l");
    crontab.start();
    crontab.waitForFinished();
    QString arr = crontab.readAll();
    return false;
}

bool AlarmModel::save()
{
    QProcess crontab;
    crontab.setProgram("crontab");
    crontab.setArguments(QStringList() << "-l");
    crontab.start();
    crontab.waitForFinished();
    QString arr = crontab.readAll();
    QStringList strarr = arr.split('\n');
    QMutableListIterator<QString> iter(strarr);
    while(iter.hasNext()) {
        if(iter.next().contains("#kirigamiclock")) {
            iter.remove();
            iter.next();
            iter.remove();
        }
    }

    for(Alarm* a : alarmsList) {
        strarr.append(QString("#kirigamiclock \"%1\"").arg(a->getName()));
        strarr.append(QString("%1").arg(a->getCronString()));
    }
    QString out;
    out = strarr.join('\n');

    crontab.setArguments(QStringList() << "-");
    crontab.start();
    //crontab.write(out);
    crontab.closeWriteChannel();
    crontab.waitForFinished();

    return false;
}

QString AlarmModel::getCrontabUpdate(const QString crontab)
{
    QStringList strarr = crontab.split('\n');
    QMutableListIterator<QString> iter(strarr);
    while(iter.hasNext()) {
        if(iter.next().contains("#kirigamiclock")) {
            iter.remove();
            iter.next();
            iter.remove();
        }
    }

    for(Alarm* a : alarmsList) {
        strarr.append(QString("#kirigamiclock \"%1\"").arg(a->getName()));
        strarr.append(QString("%1").arg(a->getCronString()));
    }
    QString out;
    out = strarr.join('\n');
    return out;
}
