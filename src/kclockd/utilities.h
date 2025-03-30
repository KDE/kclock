/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "alarmwaitworker.h"

#include <QDBusInterface>
#include <QObject>
#include <QStringList>

#include <chrono>

const QString POWERDEVIL_SERVICE_NAME = QStringLiteral("org.kde.Solid.PowerManagement");

class QTimer;
class AbstractWakeupProvider;

class Utilities : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.PowerManagement")

public:
    static void disablePowerDevil(bool disable);
    static Utilities &instance();

    bool hasPowerDevil();

    int scheduleWakeup(quint64 timestamp);
    void clearWakeup(int cookie);
    void exitAfterTimeout();
    void incfActiveCount();
    void decfActiveCount();

    static void wakeupNow();

    static void pauseMprisSources();
    static void resumeMprisSources();

    static QString formatDuration(const std::chrono::seconds &duration);

Q_SIGNALS:
    void wakeup(int cookie);
    void needsReschedule();

public Q_SLOTS:
    Q_SCRIPTABLE void wakeupCallback(int cookie);
    Q_SCRIPTABLE void keepAlive();
    Q_SCRIPTABLE void canExit();

private:
    explicit Utilities(QObject *parent = nullptr);

    void initWakeupProvider();
    bool hasWakeup();
    QDBusInterface *m_interface = nullptr;

    bool m_hasPowerDevil;
    std::atomic<int> m_activeTimerCount{0};
    QTimer *m_timer = nullptr;
    AbstractWakeupProvider *m_wakeupProvider = nullptr;

    // which mpris media sources were paused when the alarm/timer started ringing
    static QStringList m_pausedSources;
};
