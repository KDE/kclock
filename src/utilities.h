/*
 * Copyright 2020   Han Young <hanyoung@protonmail.com>
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
#pragma once

#include <QObject>

class QDBusInterface;
class AlarmWaitWorker;
class Utilities : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kclock.Utilities")
public:
    static Utilities &instance()
    {
        static Utilities singleton;
        return singleton;
    }

    const bool &hasPowerDevil()
    {
        return m_hasPowerDevil;
    };

    int scheduleWakeup(qint64 timestamp);
    void clearWakeup(int cookie);
Q_SIGNALS:
    void wakeup(int cookie);
public Q_SLOTS:
    void wakeupCallback(int cookie);

private:
    void schedule(); // For AlarmWaitWorker use
    explicit Utilities(QObject *parent = nullptr);

    QDBusInterface *m_interface = nullptr;

    bool m_hasPowerDevil = false;
    QList<int> m_cookies; // token for PowerDevil: https://invent.kde.org/plasma/powerdevil/-/merge_requests/13

    QList<std::tuple<int, long long>> m_list; // cookie, timestamp. For AlarmWaitWorker use
    int m_cookie = 1;                         // For AlarmWaitWorker use
    int m_currentCookie;                      // For AlarmWaitWorker use

    QThread *m_timerThread = nullptr;
    AlarmWaitWorker *m_worker = nullptr;
};
