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
    , m_interface(new QDBusInterface("org.kde.Solid.PowerManagement", "/org/kde/Solid/PowerManagement", "org.kde.Solid.PowerManagement", QDBusConnection::sessionBus(), this))
    , m_notifierItem(new KStatusNotifierItem(this))
{
    // DBus
    QDBusConnection::sessionBus().registerObject("/alarms", this, QDBusConnection::ExportScriptableContents);

    beginResetModel();

    // load alarms from config
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    for (QString key : group.keyList()) {
        QString json = group.readEntry(key, "");
        if (!json.isEmpty()) {
            Alarm *alarm = new Alarm(json, this);

            alarmsList.append(alarm);
            QDBusConnection::sessionBus().registerObject("/alarms/" + alarm->uuid().toString(QUuid::Id128), alarm, SCRIPTANDPROPERTY);
        }
    }

    endResetModel();

    // update notify icon in systemtray
    connect(this, &AlarmModel::nextAlarm, this, &AlarmModel::updateNotifierItem);
    m_notifierItem->setIconByName(QStringLiteral("clock"));
    m_notifierItem->setStandardActionsEnabled(false);
    m_notifierItem->setAssociatedWidget(nullptr);

    m_usePowerDevil = false;

    // if PowerDevil is present rely on PowerDevil to track time, otherwise we do it ourself
    if (m_interface->isValid()) {
        // test Plasma 5.20 PowerDevil schedule wakeup feature
        QDBusMessage m = QDBusMessage::createMethodCall("org.kde.Solid.PowerManagement", "/org/kde/Solid/PowerManagement", "org.freedesktop.DBus.Introspectable", "Introspect");
        QDBusReply<QString> result = QDBusConnection::sessionBus().call(m);

        if (result.isValid() && result.value().indexOf("scheduleWakeup") >= 0) { // have this feature
            m_usePowerDevil = true;
        }
    }
}

void AlarmModel::configureWakeups()
{
    // if we do not have powerdevil, use a wait worker thread instead
    if (!m_usePowerDevil) {
        m_timerThread = new QThread(this);
        m_worker = new AlarmWaitWorker();
        m_worker->moveToThread(m_timerThread);
        connect(m_worker, &AlarmWaitWorker::finished, [this] {
            // ring alarms that were scheduled for next wakeup
            for (auto *alarm : this->alarmsToBeRung) {
                alarm->ring();
            }
            this->alarmsToBeRung.clear();
        });
        m_timerThread->start();

        qDebug() << "PowerDevil not found, using wait worker thread for alarm wakeup.";
    } else {
        QDBusConnection::sessionBus().registerObject("/alarms", "org.kde.PowerManagement", this, QDBusConnection::ExportNonScriptableSlots);
        qDebug() << "PowerDevil found, using it for alarm wakeup.";
    }

    // start alarm polling
    scheduleAlarm();
}

quint64 AlarmModel::getNextAlarm()
{
    return nextAlarmTime;
}

void AlarmModel::scheduleAlarm()
{
    // if there are no alarms, return
    if (alarmsList.count() == 0) {
        return;
    }

    alarmsToBeRung.clear();

    // get the next minimum time for a wakeup (next alarm ring), and add alarms that will needed to be woken up to the list
    qint64 minTime = std::numeric_limits<qint64>::max();
    for (auto *alarm : alarmsList) {
        if (alarm->nextRingTime() > 0) {
            if (alarm->nextRingTime() == minTime) {
                alarmsToBeRung.append(alarm);
            } else if (alarm->nextRingTime() < minTime) {
                alarmsToBeRung.clear();
                alarmsToBeRung.append(alarm);
                minTime = alarm->nextRingTime();
            }
        }
    }

    // if there is an alarm that needs to ring
    if (minTime != std::numeric_limits<qint64>::max()) {
        qDebug() << "scheduled wakeup" << QDateTime::fromSecsSinceEpoch(minTime).toString();
        nextAlarmTime = minTime;

        if (m_usePowerDevil) {
            // if we scheduled wakeup before, cancel it first
            if (m_cookie > 0) {
                m_interface->call("clearWakeup", m_cookie);
            }

            // schedule wakeup and store cookie
            QDBusReply<uint> reply = m_interface->call("scheduleWakeup", "org.kde.kclock", QDBusObjectPath("/alarms"), (qulonglong) minTime);
            m_cookie = reply.value();

            if (!reply.isValid()) {
                qDebug() << "DBus error:" << reply.error();
            }
        } else {
            m_worker->setNewTime(minTime);
        }
    } else {
        // this don't explicitly cancel the alarm currently waiting in m_worker if disabled by user
        // because alarm->ring() will return immediately if disabled
        qDebug() << "no alarm to ring";

        nextAlarmTime = 0;
        if (m_usePowerDevil) {
            m_interface->call("clearWakeup", m_cookie);
        }
    }
    emit nextAlarm(nextAlarmTime);
}

void AlarmModel::wakeupCallback(int cookie)
{
    qDebug() << "wakeup callback";
    if (m_cookie != cookie) {
        // something must be wrong here, return and do nothing
        return;
    }

    if (!alarmsToBeRung.empty()) {
        // neutralise token
        m_cookie = -1;

        // ring alarms that were scheduled for next wakeup
        for (auto *alarm : alarmsToBeRung) {
            alarm->ring();
        }
        alarmsToBeRung.clear();
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
    // find alarm index
    int index = 0;
    bool found = false;
    for (auto id : alarmsList) {
        if (id->uuid().toString() == uuid) {
            found = true;
            break;
        }
        index++;
    }
    if (!found)
        return;

    this->remove(index);
}

void AlarmModel::remove(int index)
{
    if (index < 0 || index >= this->rowCount({}))
        return;

    emit beginRemoveRows(QModelIndex(), index, index);

    Alarm *alarmPointer = alarmsList[index];

    // remove from list of alarms to ring
    for (int i = 0; i < alarmsToBeRung.size(); i++) {
        if (alarmsToBeRung[i] == alarmPointer) {
            alarmsToBeRung.removeAt(i);
            i--;
        }
    }

    // write to config
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    group.deleteEntry(alarmsList.at(index)->uuid().toString());
    alarmsList[index]->deleteLater(); // delete object
    alarmsList.removeAt(index);

    config->sync();

    emit endRemoveRows();
}

void AlarmModel::updateUi()
{
    emit dataChanged(createIndex(0, 0), createIndex(alarmsList.count() - 1, 0));
}

void AlarmModel::addAlarm(int hours, int minutes, int daysOfWeek, QString name, QString ringtonePath)
{
    Alarm *alarm = new Alarm(this, name, minutes, hours, daysOfWeek);

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

    alarm->setRingtone(ringtonePath);
    alarmsList.insert(i, alarm);
    emit endInsertRows();

    scheduleAlarm();
    QDBusConnection::sessionBus().registerObject("/alarms/" + alarm->uuid().toString(QUuid::Id128), alarm, SCRIPTANDPROPERTY);
}

void AlarmModel::updateNotifierItem(quint64 time)
{
    if (time == 0) {
        m_notifierItem->setStatus(KStatusNotifierItem::Passive);
        m_notifierItem->setToolTip(QStringLiteral("clock"), QStringLiteral("KClock"), QStringLiteral());
    } else {
        auto dateTime = QDateTime::fromSecsSinceEpoch(time).toLocalTime();
        m_notifierItem->setStatus(KStatusNotifierItem::Active);
        m_notifierItem->setToolTip(QStringLiteral("clock"), QStringLiteral("KClock"), xi18nc("@info", "Alarm: <shortcut>%1</shortcut>", dateTime.toString("ddd ") + QLocale::system().toString(dateTime.time(), QLocale::ShortFormat)));
    }
}
