/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "utilities.h"
#include "alarmmodel.h"
#include "powerdevilwakeupprovider.h"
#include "timermodel.h"
#include "waittimerwakeupprovider.h"

#include <QApplication>
#include <QDBusReply>
#include <QDBusServiceWatcher>
#include <QDebug>
#include <QThread>
#include <QTimer>

#include "generated/systeminterfaces/mprisplayer.h"

#include <timer.h>

using namespace Qt::Literals::StringLiterals;

// manually disable powerdevil, even if found
static bool noPowerDevil = false;

QStringList Utilities::m_pausedSources;

void Utilities::disablePowerDevil(bool disable)
{
    noPowerDevil = disable;
}

Utilities &Utilities::instance()
{
    static Utilities singleton;
    return singleton;
}

Utilities::Utilities(QObject *parent)
    : QObject(parent)
    , m_interface(new QDBusInterface(POWERDEVIL_SERVICE_NAME,
                                     QStringLiteral("/org/kde/Solid/PowerManagement"),
                                     QStringLiteral("org.kde.Solid.PowerManagement"),
                                     QDBusConnection::sessionBus(),
                                     this))
    , m_hasPowerDevil(false)
    , m_kclockAlive(false)
    , m_wakeupProvider(nullptr)
{
    initWakeupProvider();

    bool success = QDBusConnection::sessionBus().registerObject(QStringLiteral("/Utility"),
                                                                QStringLiteral("org.kde.PowerManagement"),
                                                                this,
                                                                QDBusConnection::ExportScriptableSlots);
    qDebug() << "Registered on DBus:" << success;

    if (!noPowerDevil) { // do not watch for powerdevil if we explicitly turned it off
        auto m_watcher = new QDBusServiceWatcher{POWERDEVIL_SERVICE_NAME,
                                                 QDBusConnection::sessionBus(),
                                                 QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration};
        connect(m_watcher, &QDBusServiceWatcher::serviceRegistered, this, [this]() {
            qDebug() << "PowerDevil found on DBus";
            m_hasPowerDevil = hasWakeup();
            if (m_hasPowerDevil) {
                m_wakeupProvider->deleteLater();
                m_wakeupProvider = new PowerDevilWakeupProvider(this);
                connect(m_wakeupProvider, &AbstractWakeupProvider::wakeup, this, &Utilities::wakeup);
            }

            Q_EMIT needsReschedule();
        });
        connect(m_watcher, &QDBusServiceWatcher::serviceUnregistered, this, [this]() {
            qDebug() << "PowerDevil left DBus";
            m_hasPowerDevil = false;
            m_wakeupProvider->deleteLater();
            m_wakeupProvider = new WaitTimerWakeupProvider(this);
            connect(m_wakeupProvider, &AbstractWakeupProvider::wakeup, this, &Utilities::wakeup);

            Q_EMIT needsReschedule();
        });
    }
}

void Utilities::initWakeupProvider()
{
    // if PowerDevil is present, we can rely on PowerDevil to track time, otherwise we do it ourself
    if (m_interface->isValid() && !noPowerDevil) {
        m_hasPowerDevil = hasWakeup();
    }

    if (hasPowerDevil()) {
        m_wakeupProvider = new PowerDevilWakeupProvider(this);
        qDebug() << "PowerDevil found, using it for time tracking.";
    } else {
        m_wakeupProvider = new WaitTimerWakeupProvider(this);
        qDebug() << "PowerDevil not found, using wait worker thread for time tracking.";
    }
    connect(m_wakeupProvider, &AbstractWakeupProvider::wakeup, this, &Utilities::wakeup);
}

bool Utilities::hasPowerDevil()
{
    return m_hasPowerDevil;
}

int Utilities::scheduleWakeup(quint64 timestamp)
{
    return m_wakeupProvider->scheduleWakeup(timestamp);
}

void Utilities::clearWakeup(int cookie)
{
    m_wakeupProvider->clearWakeup(cookie);
}

bool Utilities::hasWakeup()
{
    QDBusMessage m = QDBusMessage::createMethodCall(QStringLiteral("org.kde.Solid.PowerManagement"),
                                                    QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                    QStringLiteral("org.freedesktop.DBus.Introspectable"),
                                                    QStringLiteral("Introspect"));
    QDBusReply<QString> result = QDBusConnection::sessionBus().call(m);

    return result.isValid() && result.value().indexOf(QStringLiteral("scheduleWakeup")) >= 0;
}

void Utilities::checkForExit()
{
    if (m_kclockAlive) {
        return;
    }
    for (auto *alarm : AlarmModel::instance()->alarmsList()) {
        if (alarm->enabled()) {
            return;
        }
    }
    for (auto *timer : TimerModel::instance()->timerList()) {
        if (timer->running() || timer->ringing()) {
            return;
        }
    }
    QApplication::exit();
}

void Utilities::wakeupNow()
{
    QDBusMessage wakeupCall = QDBusMessage::createMethodCall(QStringLiteral("org.kde.Solid.PowerManagement"),
                                                             QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                             QStringLiteral("org.kde.Solid.PowerManagement"),
                                                             QStringLiteral("wakeup"));
    QDBusConnection::sessionBus().call(wakeupCall);
}

void Utilities::wakeupCallback(int cookie)
{
    if (m_wakeupProvider) {
        qDebug() << "Utilities::wakeupCallback called with cookie:" << cookie;
        static_cast<PowerDevilWakeupProvider *>(m_wakeupProvider)->wakeupCallback(cookie);
    } else {
        qWarning() << "Wakeup provider not initialized";
    }
}

void Utilities::keepAlive()
{
    m_kclockAlive = true;
}

void Utilities::canExit()
{
    m_kclockAlive = false;
    checkForExit();
}

void Utilities::pauseMprisSources()
{
    const QStringList interfaces = QDBusConnection::sessionBus().interface()->registeredServiceNames().value();
    for (const QString &interface : interfaces) {
        if (interface.startsWith(QLatin1String("org.mpris.MediaPlayer2"))) {
            OrgMprisMediaPlayer2PlayerInterface mprisInterface(interface, QStringLiteral("/org/mpris/MediaPlayer2"), QDBusConnection::sessionBus());
            QString status = mprisInterface.playbackStatus();
            if (status == QLatin1String("Playing")) {
                if (!m_pausedSources.contains(interface)) {
                    m_pausedSources.append(interface);
                    if (mprisInterface.canPause()) {
                        mprisInterface.Pause();
                    } else {
                        mprisInterface.Stop();
                    }
                }
            }
        }
    }
}

void Utilities::resumeMprisSources()
{
    for (const QString &interface : std::as_const(m_pausedSources)) {
        OrgMprisMediaPlayer2PlayerInterface mprisInterface(interface, QStringLiteral("/org/mpris/MediaPlayer2"), QDBusConnection::sessionBus());
        mprisInterface.Play();
    }

    m_pausedSources.clear();
}

QString Utilities::formatDuration(const std::chrono::seconds &duration)
{
    const auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration - hours);
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration - hours - minutes);

    return hours.count() > 0 ? u"%1:%2:%3"_s.arg(hours.count()).arg(minutes.count(), 2, 10, '0'_L1).arg(seconds.count(), 2, 10, '0'_L1)
                             : u"%1:%2"_s.arg(minutes.count()).arg(seconds.count(), 2, 10, '0'_L1);
}

#include "moc_utilities.cpp"
