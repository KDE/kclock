/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "alarmmodel.h"

#include "alarmmodeladaptor.h"
#include "utilities.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <QDBusConnection>
#include <QDBusReply>
#include <QDebug>
#include <QLocale>

#define SCRIPTANDPROPERTY QDBusConnection::ExportScriptableContents | QDBusConnection::ExportAllProperties
AlarmModel::AlarmModel(QObject *parent)
    : QObject(parent)
    , m_notifierItem(new KStatusNotifierItem(this))
{
    // DBus
    new AlarmModelAdaptor(this);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/Alarms"), this);

    // load alarms from config
    load();

    // update notify icon in systemtray
    connect(this, &AlarmModel::nextAlarm, this, &AlarmModel::updateNotifierItem);
    m_notifierItem->setIconByName(QStringLiteral("clock"));
    m_notifierItem->setStandardActionsEnabled(false);
    m_notifierItem->setAssociatedWidget(nullptr);

    // alarm wakeup behaviour
    connect(&Utilities::instance(), &Utilities::wakeup, this, &AlarmModel::wakeupCallback);
    connect(&Utilities::instance(), &Utilities::needsReschedule, this, &AlarmModel::scheduleAlarm);
}

void AlarmModel::load()
{
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    for (QString key : group.keyList()) {
        QString json = group.readEntry(key, QStringLiteral());
        if (!json.isEmpty()) {
            Alarm *alarm = new Alarm(json, this);

            m_alarmsList.append(alarm);
        }
    }
}

void AlarmModel::configureWakeups()
{
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
        m_nextAlarmTime = 0;
        Q_EMIT nextAlarm(0);
        return;
    }

    alarmsToBeRung.clear();

    // get the next minimum time for a wakeup (next alarm ring), and add alarms that will needed to be woken up to the list
    quint64 minTime = std::numeric_limits<quint64>::max();
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

    // if there is an alarm that needs to rung
    if (minTime != std::numeric_limits<quint64>::max()) {
        qDebug() << "scheduled wakeup" << QDateTime::fromSecsSinceEpoch(minTime).toString();
        m_nextAlarmTime = minTime;

        // if we scheduled a wakeup before, cancel it first
        if (m_cookie > 0) {
            Utilities::instance().clearWakeup(m_cookie);
        }

        m_cookie = Utilities::instance().scheduleWakeup(minTime);
    } else {
        // this doesn't explicitly cancel the alarm currently waiting in m_worker if disabled by user
        // because alarm->ring() will return immediately if disabled
        qDebug() << "no alarm to ring";

        m_nextAlarmTime = 0;
        Utilities::instance().clearWakeup(m_cookie);
        m_cookie = -1;
    }
    Q_EMIT nextAlarm(m_nextAlarmTime);
}

void AlarmModel::wakeupCallback(int cookie)
{
    if (this->m_cookie == cookie) {
        for (auto alarm : this->alarmsToBeRung) {
            alarm->ring();
        }
        this->scheduleAlarm();
    }
}
void AlarmModel::removeAlarm(QString uuid)
{
    // find index of alarm
    int index = -1;
    for (int i = 0; i < m_alarmsList.size(); i++) {
        if (m_alarmsList[i]->uuid().toString() == uuid) {
            index = i;
            break;
        }
    }
    if (index != -1) {
        this->removeAlarm(index);
    }
}

void AlarmModel::removeAlarm(int index)
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
    alarm->save();

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
                                   xi18nc("@info",
                                          "Alarm: <shortcut>%1 %2</shortcut>",
                                          QLocale::system().standaloneDayName(dateTime.date().dayOfWeek()),
                                          QLocale::system().toString(dateTime.time(), QLocale::ShortFormat)));
    }
}
