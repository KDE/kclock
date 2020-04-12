/*
 * Copyright 2019  Nick Reitemeyer <nick.reitemeyer@web.de>
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

#include "alarmsmodel.h"
#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include <QMutableListIterator>

Alarm::Alarm(QObject* parent) : QObject(parent)
{
    name = "test";
    enabled = true;
    uuid = QUuid::createUuid();
}

QString Alarm::getCronString()
{
    return QString("0 18 * * * %1 --cron %2")
        .arg(QCoreApplication::applicationFilePath(), uuid.toString());
}

int AlarmModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return mList.size();
}

QVariant AlarmModel::data(const QModelIndex & index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        return mList.at(index.row())->getName();
    case EnabledRole:
        return mList.at(index.row())->getEnabled();
    default:
        return QVariant();
    }
}

bool AlarmModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid()) {
        switch (role) {
        case NameRole:
            if (value.type() == QVariant::String) {
                mList[index.row()]->setName(value.toString());
                emit dataChanged(index, index, QVector<int> { NameRole });
                return true;
            }
            return false;
        case EnabledRole:
            if(value.type() == QVariant::Bool) {
                mList[index.row()]->setEnabled(value.toBool());
                emit dataChanged(index, index, QVector<int> { EnabledRole });
                return true;
            }
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
    roles[NameRole] = "name";
    roles[EnabledRole] = "enabled";
    roles[RepeatedRole] = "repeated";
    return roles;
}

Alarm* AlarmModel::addAlarm()
{
    beginInsertRows(QModelIndex(), rowCount(QModelIndex()), rowCount(QModelIndex()));
    Alarm* ret = new Alarm(this);
    mList.append(ret);
    endInsertRows();
    return ret;
}

bool AlarmModel::load()
{
    QProcess crontab;
    crontab.setProgram("crontab");
    crontab.setArguments(QStringList() << "-l");
    crontab.start();
    crontab.waitForFinished();
    QString arr = crontab.readAll();
    qDebug() << arr;
    
    qDebug() << getCrontabUpdate(arr);
    qDebug() << getCrontabUpdate(getCrontabUpdate(arr) + "\nhello\n");
    return false;
}

bool AlarmModel::save()
{
    /*bool dirty = false;
    for (Alarm* a : mList) {
        if (a->isDirty()) {
            dirty = true;
            break;
        }
    }
    if (!dirty) return false;*/
    
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
    
    for(Alarm* a : mList) {
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
    qDebug() << crontab.exitCode();
    
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
    
    for(Alarm* a : mList) {
        strarr.append(QString("#kirigamiclock \"%1\"").arg(a->getName()));
        strarr.append(QString("%1").arg(a->getCronString()));
    }
    QString out;
    out = strarr.join('\n');
    return out;
}
