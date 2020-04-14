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
#include "alarms.h"

Alarm::Alarm(QObject *parent, QString name, int minutes, int hours, QString dayOfWeek) {
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

int AlarmModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return alarmsList.size();
}

QVariant AlarmModel::data(const QModelIndex & index, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            return QVariant::fromValue(alarmsList.at(index.row()));
        case EnabledRole:
            return alarmsList.at(index.row())->isEnabled();
        default:
            return QVariant();
    }
}

bool AlarmModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid()) {
        switch (role) {
            case EnabledRole:
                if  (value.type() == QVariant::Bool) {
                    alarmsList[index.row()]->setEnabled(value.toBool());
                    emit dataChanged(index, index, QVector<int> { EnabledRole });
                    return true;
                }
                break;
        }
    }
    return false;
}

Qt::ItemFlags AlarmModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEditable;
}

QHash<int, QByteArray> AlarmModel::roleNames() const
{
    auto roles = QHash<int, QByteArray>();
    roles[EnabledRole] = "enabled";
    roles[Qt::DisplayRole] = "object";
    return roles;
}

Alarm* AlarmModel::addAlarm(QString name, int minutes, int hours, QString dayOfWeek)
{
    beginInsertRows(QModelIndex(), 0, 0);
    auto* alarm = new Alarm(this, name, minutes, hours, dayOfWeek);
    alarmsList.prepend(alarm);
    endInsertRows();
    return alarm;
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