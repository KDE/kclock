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
#include <QThread>
#include <QXmlStreamReader>

#include <klocalizedstring.h>

#include "alarmmodel.h"
#include "alarms.h"
#include "kclock_algorithm.hpp"

AlarmModel::AlarmModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_interface(new org::kde::kclock::AlarmModel(QStringLiteral("org.kde.kclockd"), QStringLiteral("/alarms"), QDBusConnection::sessionBus(), this))
{
    if (m_interface->isValid()) {
        connect(m_interface, SIGNAL(alarmAdded(QString)), this, SLOT(addAlarm(QString)));
        connect(m_interface, SIGNAL(alarmRemoved(QString)), this, SLOT(removeAlarm(QString)));
    }
    QDBusInterface *interface = new QDBusInterface(QStringLiteral("org.kde.kclockd"), QStringLiteral("/alarms"), QStringLiteral("org.freedesktop.DBus.Introspectable"), QDBusConnection::sessionBus(), this);
    QDBusReply<QString> reply = interface->call(QStringLiteral("Introspect"));
    if (reply.isValid()) {
        auto xmlMsg = reply.value();
        QXmlStreamReader xml(xmlMsg);
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.name() == QStringLiteral("node") && xml.attributes().hasAttribute(QStringLiteral("name"))) {
                if (xml.attributes().value(QStringLiteral("name")).toString().indexOf(QStringLiteral("org")) == -1) {
                    this->addAlarm(xml.attributes().value(QStringLiteral("name")).toString());
                }
            }
        }
    }
    interface->deleteLater();
}
/* ~ Alarm row data ~ */

QHash<int, QByteArray> AlarmModel::roleNames() const
{
    return {{HoursRole, "hours"}, {MinutesRole, "minutes"}, {NameRole, "name"}, {EnabledRole, "enabled"}, {DaysOfWeekRole, "daysOfWeek"}, {AlarmRole, "alarm"}};
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

    m_interface->remove(alarmsList.at(index)->uuid().toString());
    auto ptr = alarmsList.at(index);

    emit beginRemoveRows(QModelIndex(), index, index);
    alarmsList.removeAt(index);
    emit endRemoveRows();

    ptr->deleteLater();
}

void AlarmModel::updateUi()
{
    emit dataChanged(createIndex(0, 0), createIndex(alarmsList.count() - 1, 0));
}

Alarm *AlarmModel::addAlarm(int hours, int minutes, int daysOfWeek, QString name, QString ringtonePath)
{
    // TODO: add alarm to remote
    return {};
}

void AlarmModel::addAlarm(QString uuid)
{
    auto alarm = new Alarm(uuid.remove(QRegularExpression(QStringLiteral("[{}-]"))));
    auto index = KClock::insert_index(alarm, alarmsList, [](Alarm *const &left, Alarm *const &right) {
        if (left->hours() < right->hours())
            return true;
        else if (left->hours() > right->hours())
            return false;
        else if (left->minutes() <= right->minutes())
            return true;
        else
            return false;
    });

    Q_EMIT beginInsertRows(QModelIndex(), index, index);
    alarmsList.insert(index, alarm);
    Q_EMIT endInsertRows();
}

void AlarmModel::removeAlarm(QString uuid)
{
    auto index = 0;
    for (auto alarm : alarmsList) {
        if (alarm->uuid().toString() == uuid) {
            break;
        }
        ++index;
    }

    auto ptr = alarmsList.at(index);

    emit beginRemoveRows(QModelIndex(), index, index);
    alarmsList.removeAt(index);
    emit endRemoveRows();

    ptr->deleteLater();
}
