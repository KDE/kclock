/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "timer.h"

#include "utilities.h"

#include <KLocalizedString>
#include <KNotification>

#include <QDBusConnection>
#include <QJsonObject>

/* ~ Timer ~ */

Timer::Timer(int length, QString label, bool running)
    : m_uuid(QUuid::createUuid())
    , m_length(length)
    , m_label(label)
{
    connect(&Utilities::instance(), &Utilities::wakeup, this, &Timer::timeUp);
    connect(&Utilities::instance(), &Utilities::needsReschedule, this, &Timer::reschedule);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/Timers/") + this->m_uuid.toString(QUuid::Id128),
                                                 this,
                                                 QDBusConnection::ExportScriptableContents | QDBusConnection::ExportAllProperties);
    connect(this, &QObject::destroyed, [this] {
        QDBusConnection::sessionBus().unregisterObject(QStringLiteral("/Timers/") + this->m_uuid.toString(QUuid::Id128), QDBusConnection::UnregisterNode);
    });
    if (running)
        this->toggleRunning();
}

Timer::Timer(const QJsonObject &obj)
{
    m_length = obj[QStringLiteral("length")].toInt();
    m_label = obj[QStringLiteral("label")].toString();
    m_uuid = QUuid(obj[QStringLiteral("uuid")].toString());

    connect(&Utilities::instance(), &Utilities::wakeup, this, &Timer::timeUp);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/Timers/") + this->m_uuid.toString(QUuid::Id128),
                                                 this,
                                                 QDBusConnection::ExportScriptableContents | QDBusConnection::ExportAllProperties);
    connect(this, &QObject::destroyed, [this] {
        QDBusConnection::sessionBus().unregisterObject(QStringLiteral("/Timers/") + this->m_uuid.toString(QUuid::Id128), QDBusConnection::UnregisterNode);
    });
}

Timer::~Timer()
{
    if (!m_running) { // stop wakeup if timer is being deleted
        setRunning(false);
    }
}

QJsonObject Timer::serialize()
{
    QJsonObject obj;
    obj[QStringLiteral("length")] = m_length;
    obj[QStringLiteral("label")] = m_label;
    obj[QStringLiteral("uuid")] = m_uuid.toString();
    return obj;
}

void Timer::toggleRunning()
{
    setRunning(!m_running);
}

void Timer::reset()
{
    setRunning(false);
    m_hasElapsed = 0;

    Q_EMIT runningChanged();
}

void Timer::timeUp(int cookie)
{
    if (cookie == m_cookie) {
        this->sendNotification();
        this->m_cookie = -1;
        this->setRunning(false);
        this->m_hasElapsed = m_length;
    }
}

void Timer::setRunning(bool running)
{
    if (m_running == running)
        return;

    if (m_running) {
        m_hasElapsed = QDateTime::currentSecsSinceEpoch() - m_startTime;

        if (m_cookie > 0) {
            Utilities::instance().clearWakeup(m_cookie);
            m_cookie = -1;
        }
    } else {
        if (m_hasElapsed == m_length) { // reset elapsed if the timer was already finished
            m_hasElapsed = 0;
        }

        m_startTime = QDateTime::currentSecsSinceEpoch() - m_hasElapsed;
        m_cookie = Utilities::instance().scheduleWakeup(m_startTime + m_length);
    }

    m_running = running;

    Q_EMIT runningChanged();

    TimerModel::instance()->save();
}

void Timer::sendNotification()
{
    qDebug("Timer finished, sending notification...");

    KNotification *notif = new KNotification("timerFinished");
    notif->setIconName(QStringLiteral("kclock"));
    notif->setTitle(i18n("Timer complete"));
    notif->setText(i18n("Your timer %1 has finished!", this->label()));
    notif->setDefaultAction(i18n("View"));
    notif->setUrgency(KNotification::HighUrgency);
    notif->setFlags(KNotification::NotificationFlag::LoopSound | KNotification::NotificationFlag::Persistent);
    connect(notif, &KNotification::closed, [notif] {
        notif->close();
    });

    notif->sendEvent();
}
void Timer::reschedule()
{
    if (m_running) {
        m_cookie = Utilities::instance().scheduleWakeup(m_startTime + m_length);
    }
}
