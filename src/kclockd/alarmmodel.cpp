/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
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

AlarmModel *AlarmModel::instance()
{
    static AlarmModel *singleton = new AlarmModel();
    return singleton;
}

AlarmModel::AlarmModel(QObject *parent)
    : QObject{parent}
{
    // DBus
    new AlarmModelAdaptor(this);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/Alarms"), this);

    // load alarms from config
    load();

    // update notify icon in systemtray
    connect(this, &AlarmModel::nextAlarm, this, &AlarmModel::updateNotifierItem);

    // alarm wakeup behaviour
    connect(&Utilities::instance(), &Utilities::wakeup, this, &AlarmModel::wakeupCallback);
    connect(&Utilities::instance(), &Utilities::needsReschedule, this, &AlarmModel::scheduleAlarm);
}

void AlarmModel::load()
{
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    QStringList list = group.keyList();

    for (const QString &key : list) {
        QString json = group.readEntry(key, QString());
        if (!json.isEmpty()) {
            Alarm *alarm = new Alarm(json, this);
            m_alarmsList.append(alarm);
        }
    }
}

void AlarmModel::save()
{
    std::for_each(m_alarmsList.begin(), m_alarmsList.end(), [](Alarm *alarm) {
        alarm->save();
    });
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

    alarmsToRing.clear();

    // get the next minimum time for a wakeup (next alarm ring), and add alarms that will needed to be woken up to the list
    quint64 minTime = std::numeric_limits<quint64>::max();
    for (auto *alarm : std::as_const(m_alarmsList)) {
        if (alarm->nextRingTime() > 0) {
            if (alarm->nextRingTime() == minTime) {
                alarmsToRing.append(alarm);
            } else if (alarm->nextRingTime() < minTime) {
                alarmsToRing.clear();
                alarmsToRing.append(alarm);
                minTime = alarm->nextRingTime();
            }
        }
    }

    int activeAlarmCount = alarmsToRing.size();
    while (activeAlarmCount--) {
        Utilities::instance().incfActiveCount();
    }

    // if there is an alarm that needs to rung
    if (minTime != std::numeric_limits<quint64>::max()) {
        qDebug() << "Scheduled alarm wakeup at" << QDateTime::fromSecsSinceEpoch(minTime).toString() << ".";
        m_nextAlarmTime = minTime;

        // if we scheduled a wakeup before, cancel it first
        if (m_cookie > 0) {
            Utilities::instance().clearWakeup(m_cookie);
        }

        m_cookie = Utilities::instance().scheduleWakeup(minTime);
    } else {
        // this doesn't explicitly cancel the alarm currently waiting in m_worker if disabled by user
        // because alarm->ring() will return immediately if disabled
        qDebug() << "No alarms scheduled.";

        m_nextAlarmTime = 0;
        Utilities::instance().clearWakeup(m_cookie);
        m_cookie = -1;
    }
    Q_EMIT nextAlarm(m_nextAlarmTime);
}

void AlarmModel::wakeupCallback(int cookie)
{
    if (m_cookie == cookie) {
        for (auto alarm : std::as_const(alarmsToRing)) {
            alarm->ring();
        }
        scheduleAlarm();
    }
}
void AlarmModel::removeAlarm(QString uuid)
{
    for (int i = 0; i < m_alarmsList.size(); i++) {
        if (m_alarmsList[i]->uuid() == uuid) {
            removeAlarm(i);
            break;
        }
    }
}

void AlarmModel::removeAlarm(int index)
{
    if (index < 0 || index >= this->m_alarmsList.size()) {
        return;
    }

    Q_EMIT alarmRemoved(m_alarmsList.at(index)->uuid());

    Alarm *alarmPointer = m_alarmsList.at(index);

    // remove from list of alarms to ring
    for (int i = 0; i < alarmsToRing.size(); i++) {
        if (alarmsToRing.at(i) == alarmPointer) {
            alarmsToRing.removeAt(i);
            i--;
        }
    }

    // write to config
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    group.deleteEntry(m_alarmsList.at(index)->uuid());
    m_alarmsList.at(index)->deleteLater(); // delete object
    m_alarmsList.removeAt(index);

    config->sync();
    scheduleAlarm();
}

void AlarmModel::addAlarm(QString name, int hours, int minutes, int daysOfWeek, QString audioPath, int ringDuration, int snoozeDuration)
{
    Alarm *alarm = new Alarm(name, hours, minutes, daysOfWeek, audioPath, ringDuration, snoozeDuration, this);
    alarm->save();

    // insert new alarm in order by time of day
    int i = 0;
    while (i < m_alarmsList.size() && !(m_alarmsList[i]->hours() == hours && m_alarmsList[i]->minutes() >= minutes)) {
        ++i;
    }
    m_alarmsList.insert(i, alarm);

    scheduleAlarm();
    Q_EMIT alarmAdded(alarm->uuid());
}

void AlarmModel::initNotifierItem()
{
    if (!m_item) {
        m_item = new KStatusNotifierItem(this);
        m_item->setIconByName(QStringLiteral("clock"));
        m_item->setStandardActionsEnabled(false);
        m_item->setAssociatedWidget(nullptr);
        m_item->setCategory(KStatusNotifierItem::SystemServices);
        m_item->setStatus(KStatusNotifierItem::Passive);
    }
}

void AlarmModel::updateNotifierItem(quint64 time)
{
    if (time == 0) {
        // no alarm waiting, only set notifier if we have one
        if (m_item) {
            m_item->setStatus(KStatusNotifierItem::Passive);
            m_item->setToolTip(QStringLiteral("clock"), QStringLiteral("KClock"), QString());
        }
    } else {
        auto dateTime = QDateTime::fromSecsSinceEpoch(time).toLocalTime();
        initNotifierItem();
        m_item->setStatus(KStatusNotifierItem::Active);
        m_item->setToolTip(QStringLiteral("clock"),
                           QStringLiteral("KClock"),
                           xi18nc("@info",
                                  "Alarm: <shortcut>%1 %2</shortcut>",
                                  QLocale::system().standaloneDayName(dateTime.date().dayOfWeek()),
                                  QLocale::system().toString(dateTime.time(), QLocale::ShortFormat)));
    }
}
