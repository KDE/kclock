/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 * Copyright 2022 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "timer.h"
#include "utilities.h"

#include <QProcess>

#include <KLocalizedString>
#include <KNotification>

#include <QDBusConnection>
#include <QJsonObject>

Timer::Timer(int length, QString label, QString commandTimeout, bool running, QObject *parent)
    : QObject{parent}
    , m_uuid{QUuid::createUuid()}
    , m_length{length}
    , m_label{label}
    , m_commandTimeout{commandTimeout}
    , m_looping{false}
{
    init();

    // start timer if requested
    if (running) {
        this->toggleRunning();
    }
}

Timer::Timer(const QJsonObject &obj, QObject *parent)
    : QObject{parent}
    , m_uuid{QUuid(obj[QStringLiteral("uuid")].toString())}
    , m_length{obj[QStringLiteral("length")].toInt()}
    , m_label{obj[QStringLiteral("label")].toString()}
    , m_commandTimeout{obj[QStringLiteral("commandTimeout")].toString()}
    , m_looping{obj[QStringLiteral("looping")].toBool()}
{
    init();
}

Timer::~Timer()
{
    if (!m_running) {
        // stop wakeup if timer is being deleted
        setRunning(false);
    }
}

void Timer::init()
{
    // connect signals
    connect(&Utilities::instance(), &Utilities::wakeup, this, &Timer::timeUp);
    connect(&Utilities::instance(), &Utilities::needsReschedule, this, &Timer::reschedule);

    // initialize DBus object
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/Timers/") + m_uuid.toString(QUuid::Id128),
                                                 this,
                                                 QDBusConnection::ExportScriptableContents | QDBusConnection::ExportAllProperties);
    connect(this, &QObject::destroyed, [this] {
        QDBusConnection::sessionBus().unregisterObject(QStringLiteral("/Timers/") + m_uuid.toString(QUuid::Id128), QDBusConnection::UnregisterNode);
    });
}

QJsonObject Timer::serialize()
{
    QJsonObject obj;
    obj[QStringLiteral("length")] = m_length;
    obj[QStringLiteral("label")] = m_label;
    obj[QStringLiteral("uuid")] = m_uuid.toString();
    obj[QStringLiteral("looping")] = m_looping;
    obj[QStringLiteral("commandTimeout")] = m_commandTimeout;
    return obj;
}

void Timer::toggleRunning()
{
    setRunning(!m_running);
}

void Timer::toggleLooping()
{
    m_looping = !m_looping;

    Q_EMIT loopingChanged();

    TimerModel::instance()->save();
}

void Timer::reset()
{
    setRunning(false);
    m_hasElapsed = 0;

    Q_EMIT runningChanged();
}

int Timer::elapsed() const
{
    if (running()) {
        return QDateTime::currentSecsSinceEpoch() - m_startTime;
    } else {
        return m_hasElapsed;
    }
}
QString Timer::getUUID()
{
    return m_uuid.toString();
}
const QUuid &Timer::uuid() const
{
    return m_uuid;
};
const int &Timer::length() const
{
    return m_length;
}
void Timer::setLength(int length)
{
    m_length = length;
    Q_EMIT lengthChanged();
    TimerModel::instance()->save();
}

const QString &Timer::label() const
{
    return m_label;
}

void Timer::setLabel(QString label)
{
    m_label = label;
    Q_EMIT labelChanged();
    TimerModel::instance()->save();
}

const QString &Timer::commandTimeout() const
{
    return m_commandTimeout;
}

void Timer::setCommandTimeout(QString commandTimeout)
{
    m_commandTimeout = commandTimeout;
    Q_EMIT commandTimeoutChanged();
    TimerModel::instance()->save();
}

const bool &Timer::looping() const
{
    return m_looping;
}

const bool &Timer::running() const
{
    return m_running;
}

void Timer::timeUp(int cookie)
{
    if (cookie == m_cookie) {
        this->sendNotification();
        this->m_cookie = -1;
        if (m_commandTimeout.isEmpty()) {
            QProcess *process = new QProcess;
            process->start(m_commandTimeout);
        }
        if (m_looping) {
            this->reset();
            this->setRunning(true);
        } else {
            this->setRunning(false);
            this->m_hasElapsed = m_length;
        }
    }
}

void Timer::setRunning(bool running)
{
    if (m_running == running)
        return;

    if (m_running) {
        m_hasElapsed = QDateTime::currentSecsSinceEpoch() - m_startTime;
        Utilities::instance().decfActiveCount();
        if (m_cookie > 0) {
            Utilities::instance().clearWakeup(m_cookie);
            m_cookie = -1;
        }
    } else {
        if (m_hasElapsed == m_length) { // reset elapsed if the timer was already finished
            m_hasElapsed = 0;
        }
        Utilities::instance().incfActiveCount();
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

    KNotification *notif = new KNotification(QStringLiteral("timerFinished"));
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
