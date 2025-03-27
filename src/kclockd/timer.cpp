/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 * Copyright 2022 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "timer.h"
#include "alarmplayer.h"

#include <QFile>
#include <QStandardPaths>

Timer::Timer(int length, const QString &label, const QString &commandTimeout, bool running, QObject *parent)
    : QObject{parent}
    , m_uuid{QUuid::createUuid()}
    , m_length{length}
    , m_label{label}
    , m_commandTimeout{commandTimeout}
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

    // ensure timer doesn't continue ringing after deletion
    if (m_ringing) {
        AlarmPlayer::instance().stop();
    }
}

void Timer::init()
{
    // connect signals
    connect(&Utilities::instance(), &Utilities::wakeup, this, &Timer::timeUp);
    connect(&Utilities::instance(), &Utilities::needsReschedule, this, &Timer::reschedule);

    // initialize notification
    m_notification->setIconName(QStringLiteral("org.kde.kclock"));
    m_notification->setTitle(i18n("Timer complete"));
    m_notification->setText(i18n("Your timer %1 has finished!", label()));
    m_notification->setUrgency(KNotification::HighUrgency);
    m_notification->setAutoDelete(false); // don't auto-delete when closing

    auto defaultAction = m_notification->addDefaultAction(i18n("View"));
    connect(defaultAction, &KNotificationAction::activated, this, &Timer::dismiss);

    connect(m_notification, &KNotification::closed, this, &Timer::dismiss);

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

    // ensure UI keeps up to date with hasElapsed
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

QString Timer::uuid() const
{
    return m_uuid.toString();
}

int Timer::length() const
{
    return m_length;
}

void Timer::setLength(int length)
{
    if (length != m_length) {
        m_length = length;
        Q_EMIT lengthChanged();

        TimerModel::instance()->save();
        reschedule();
    }
}

QString Timer::label() const
{
    return m_label;
}

void Timer::setLabel(const QString &label)
{
    if (label != m_label) {
        m_label = label;
        Q_EMIT labelChanged();

        TimerModel::instance()->save();
    }
}

QString Timer::commandTimeout() const
{
    return m_commandTimeout;
}

void Timer::setCommandTimeout(const QString &commandTimeout)
{
    if (m_commandTimeout != commandTimeout) {
        m_commandTimeout = commandTimeout;
        Q_EMIT commandTimeoutChanged();

        TimerModel::instance()->save();
    }
}

bool Timer::looping() const
{
    return m_looping;
}

bool Timer::running() const
{
    return m_running;
}

bool Timer::ringing() const
{
    return m_ringing;
}

void Timer::timeUp(int cookie)
{
    if (cookie == m_cookie) {
        ring();

        // clear wakeup if it's somehow still there
        if (m_cookie > 0) {
            Utilities::instance().clearWakeup(m_cookie);
            m_cookie = -1;
        }

        // run command since timer has ended
        qDebug() << "Running command:" << m_commandTimeout;
        if (!m_commandTimeout.isEmpty()) {
            const QStringList commandAndArguments = QProcess::splitCommand(m_commandTimeout);
            QProcess::execute(commandAndArguments.front(), commandAndArguments.mid(1));
        }

        // loop if it is set
        if (m_looping) {
            reset();
            setRunning(true);
        } else {
            setRunning(false);
            m_hasElapsed = m_length;
        }
    }
}

void Timer::setRunning(bool running)
{
    if (m_running == running) {
        return;
    }

    if (m_running) {
        m_hasElapsed = QDateTime::currentSecsSinceEpoch() - m_startTime;

        Utilities::instance().decfActiveCount();

        // clear wakeup
        if (m_cookie > 0) {
            Utilities::instance().clearWakeup(m_cookie);
            m_cookie = -1;
        }
    } else {
        if (m_hasElapsed == m_length) {
            // reset elapsed if the timer was already finished
            m_hasElapsed = 0;
        }

        // if we scheduled a wakeup before, cancel it first
        if (m_cookie > 0) {
            Utilities::instance().clearWakeup(m_cookie);
        }

        Utilities::instance().incfActiveCount();

        m_startTime = QDateTime::currentSecsSinceEpoch() - m_hasElapsed;
        m_cookie = Utilities::instance().scheduleWakeup(m_startTime + m_length);
    }

    m_running = running;
    Q_EMIT runningChanged();

    TimerModel::instance()->save();
}

void Timer::ring()
{
    // if there were other ring events running, close them
    m_notification->close();

    qDebug("Timer finished, sending notification...");
    m_notification->sendEvent();

    Utilities::pauseMprisSources();

    AlarmPlayer::instance().setSource(findRingSoundLocation());
    AlarmPlayer::instance().play();

    m_ringing = true;
    Q_EMIT ringingChanged();
}

void Timer::dismiss()
{
    qDebug() << "Timer dismissed.";
    m_notification->close();

    AlarmPlayer::instance().stop();

    Utilities::resumeMprisSources();

    m_ringing = false;
    Q_EMIT ringingChanged();
}

void Timer::reschedule()
{
    if (m_running) {
        m_cookie = Utilities::instance().scheduleWakeup(m_startTime + m_length);
    }
}

QUrl Timer::findRingSoundLocation()
{
    const auto locations = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    const QString path = QStringLiteral("/sounds/freedesktop/stereo/alarm-clock-elapsed.oga");
    QString alarmSoundPath;

    for (const auto &directory : locations) {
        if (QFile(directory + path).exists()) {
            return QUrl(directory + path);
        }
    }

    return QUrl();
}
#include "moc_timer.cpp"
