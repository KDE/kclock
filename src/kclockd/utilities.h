/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KCLOCKD_UTILITIES_H
#define KCLOCKD_UTILITIES_H

#include "alarmwaitworker.h"

#include <QDBusInterface>
#include <QObject>
class QTimer;
const QString POWERDEVIL_SERVICE_NAME = QStringLiteral("org.kde.Solid.PowerManagement");
class Utilities : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.PowerManagement")
public:
    static Utilities &instance()
    {
        static Utilities singleton;
        return singleton;
    }

    bool hasPowerDevil()
    {
        return m_hasPowerDevil;
    };

    int scheduleWakeup(quint64 timestamp);
    void clearWakeup(int cookie);
    void exitAfterTimeout();
    void incfActiveCount();
    void decfActiveCount();

Q_SIGNALS:
    void wakeup(int cookie);
    void needsReschedule();
public Q_SLOTS:
    Q_SCRIPTABLE void wakeupCallback(int cookie);
    Q_SCRIPTABLE void keepAlive();
    Q_SCRIPTABLE void canExit();

private:
    explicit Utilities(QObject *parent = nullptr);

    void schedule(); // For AlarmWaitWorker use
    void initWorker();
    bool hasWakeup();
    QDBusInterface *m_interface = nullptr;

    bool m_hasPowerDevil = false;
    QList<int> m_cookies; // token for PowerDevil: https://invent.kde.org/plasma/powerdevil/-/merge_requests/13

    QList<std::tuple<int, quint64>> m_list; // cookie, timestamp. For AlarmWaitWorker use
    int m_cookie = 1; // For AlarmWaitWorker use
    int m_currentCookie; // For AlarmWaitWorker use
    std::atomic<int> m_activeTimerCount{0};

    QThread *m_timerThread = nullptr;
    AlarmWaitWorker *m_worker = nullptr;
    QTimer *m_timer = nullptr;
};

#endif // KCLOCKD_UTILITIES_H
