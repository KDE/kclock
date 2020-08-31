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
#include <klocalizedstring.h>

#include "alarmmodel.h"
#include "alarmmodeladaptor.h"
#include "alarms.h"
#include "alarmwaitworker.h"

#define SCRIPTANDPROPERTY QDBusConnection::ExportScriptableContents | QDBusConnection::ExportAllProperties
AlarmModel::AlarmModel(QObject *parent)
    : QObject(parent)
    , m_interface(new QDBusInterface(QStringLiteral("org.kde.Solid.PowerManagement"), QStringLiteral("/org/kde/Solid/PowerManagement"), QStringLiteral("org.kde.Solid.PowerManagement"), QDBusConnection::sessionBus(), this))
    , m_notifierItem(new KStatusNotifierItem(this))
{
    // DBus
    new AlarmModelAdaptor(this);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/alarms"), this);

    // load alarms from config
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    for (QString key : group.keyList()) {
        QString json = group.readEntry(key, QStringLiteral());
        if (!json.isEmpty()) {
            Alarm *alarm = new Alarm(json, this);

            m_alarmsList.append(alarm);
        }
    }

    // update notify icon in systemtray
    connect(this, &AlarmModel::nextAlarm, this, &AlarmModel::updateNotifierItem);
    m_notifierItem->setIconByName(QStringLiteral("clock"));
    m_notifierItem->setStandardActionsEnabled(false);
    m_notifierItem->setAssociatedWidget(nullptr);

    m_usePowerDevil = false;

    // if PowerDevil is present rely on PowerDevil to track time, otherwise we do it ourself
    if (m_interface->isValid()) {
        // test Plasma 5.20 PowerDevil schedule wakeup feature
        QDBusMessage m = QDBusMessage::createMethodCall(QStringLiteral("org.kde.Solid.PowerManagement"), QStringLiteral("/org/kde/Solid/PowerManagement"), QStringLiteral("org.freedesktop.DBus.Introspectable"), QStringLiteral("Introspect"));
        QDBusReply<QString> result = QDBusConnection::sessionBus().call(m);

        if (result.isValid() && result.value().indexOf(QStringLiteral("scheduleWakeup")) >= 0) { // have this feature
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
        bool success = QDBusConnection::sessionBus().registerObject(QStringLiteral("/alarmswakeup"), QStringLiteral("org.kde.PowerManagement"), this, QDBusConnection::ExportNonScriptableSlots);
        qDebug() << "PowerDevil found, using it for alarm wakeup. Success:" << success;
    }

    // start alarm polling
    scheduleAlarm();
}

quint64 AlarmModel::getNextAlarm()
{
    return m_nextAlarmTime;
}

void AlarmModel::scheduleAlarm()
{
    // if there are no alarms, return
    if (m_alarmsList.count() == 0) {
        Q_EMIT nextAlarm(0);
        return;
    }

    alarmsToBeRung.clear();

    // get the next minimum time for a wakeup (next alarm ring), and add alarms that will needed to be woken up to the list
    qint64 minTime = std::numeric_limits<qint64>::max();
    for (auto *alarm : m_alarmsList) {
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
        m_nextAlarmTime = minTime;

        if (m_usePowerDevil) {
            // if we scheduled wakeup before, cancel it first
            if (m_cookie > 0) {
                m_interface->call(QStringLiteral("clearWakeup"), m_cookie);
            }

            // schedule wakeup and store cookie
            QDBusReply<uint> reply = m_interface->call(QStringLiteral("scheduleWakeup"), QStringLiteral("org.kde.kclock"), QDBusObjectPath("/alarmswakeup"), (qulonglong)minTime);
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

        m_nextAlarmTime = 0;
        if (m_usePowerDevil) {
            m_interface->call(QStringLiteral("clearWakeup"), m_cookie);
        }
    }
    Q_EMIT nextAlarm(m_nextAlarmTime);
}

void AlarmModel::wakeupCallback(int cookie)
{
    qDebug() << "wakeup callback";
    if (m_cookie != cookie) {
        // something must be wrong here, return and do nothing
        qDebug() << "callback ignored (wrong cookie)";
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

void AlarmModel::remove(QString uuid)
{
    // find alarm index
    int index = 0;
    bool found = false;
    for (auto id : m_alarmsList) {
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
    if (index < 0 || index >= this->m_alarmsList.size())
        return;

    Q_EMIT alarmRemoved(m_alarmsList.at(index)->uuid().toString());

    Alarm *alarmPointer = m_alarmsList.at(index);

    // remove from list of alarms to ring
    for (int i = 0; i < alarmsToBeRung.size(); i++) {
        if (alarmsToBeRung.at(i) == alarmPointer) {
            alarmsToBeRung.removeAt(i);
            i--;
        }
    }

    // write to config
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    group.deleteEntry(m_alarmsList.at(index)->uuid().toString());
    m_alarmsList.at(index)->deleteLater(); // delete object
    m_alarmsList.removeAt(index);

    config->sync();
    scheduleAlarm();
}

void AlarmModel::addAlarm(int hours, int minutes, int daysOfWeek, QString name, QString ringtonePath)
{
    Alarm *alarm = new Alarm(this, name, minutes, hours, daysOfWeek);

    // insert new alarm in order by time of day
    int i = 0;
    for (auto alarms : m_alarmsList) {
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

    m_alarmsList.insert(i, alarm);

    scheduleAlarm();

    Q_EMIT alarmAdded(alarm->uuid().toString());
}

void AlarmModel::updateNotifierItem(quint64 time)
{
    if (time == 0) {
        m_notifierItem->setStatus(KStatusNotifierItem::Passive);
        m_notifierItem->setToolTip(QStringLiteral("clock"), QStringLiteral("KClock"), QStringLiteral());
    } else {
        auto dateTime = QDateTime::fromSecsSinceEpoch(time).toLocalTime();
        m_notifierItem->setStatus(KStatusNotifierItem::Active);
        m_notifierItem->setToolTip(QStringLiteral("clock"),
                                   QStringLiteral("KClock"),
                                   xi18nc("@info", "Alarm: <shortcut>%1</shortcut>", QLocale::system().standaloneDayName(dateTime.date().dayOfWeek()) + QLocale::system().toString(dateTime.time(), QLocale::ShortFormat)));
    }
}
