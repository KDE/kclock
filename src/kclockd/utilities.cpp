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
#include <QDBusReply>
#include <QDebug>
#include <QThread>

#include "utilities.h"

Utilities::Utilities(QObject *parent)
    : QObject(parent)
    , m_interface(new QDBusInterface(QStringLiteral("org.kde.Solid.PowerManagement"),
                                     QStringLiteral("/org/kde/Solid/PowerManagement"),
                                     QStringLiteral("org.kde.Solid.PowerManagement"),
                                     QDBusConnection::sessionBus(),
                                     this))
{
    // if PowerDevil is present, we can rely on PowerDevil to track time, otherwise we do it ourself
    if (m_interface->isValid()) {
        // test Plasma 5.20 PowerDevil schedule wakeup feature
        QDBusMessage m = QDBusMessage::createMethodCall(QStringLiteral("org.kde.Solid.PowerManagement"),
                                                        QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                        QStringLiteral("org.freedesktop.DBus.Introspectable"),
                                                        QStringLiteral("Introspect"));
        QDBusReply<QString> result = QDBusConnection::sessionBus().call(m);

        if (result.isValid() && result.value().indexOf(QStringLiteral("scheduleWakeup")) >= 0) { // have this feature
            m_hasPowerDevil = true;
        }
    }

    if (this->hasPowerDevil()) {
        bool success = QDBusConnection::sessionBus().registerObject(QStringLiteral("/Utility"),
                                                                    QStringLiteral("org.kde.PowerManagement"),
                                                                    this,
                                                                    QDBusConnection::ExportScriptableSlots);
        qDebug() << "PowerDevil found, using it for time tracking. Success:" << success;
    } else {
        m_timerThread = new QThread(this);
        m_worker = new AlarmWaitWorker();
        m_worker->moveToThread(m_timerThread);
        connect(m_worker, &AlarmWaitWorker::finished, [this] {
            // notify time is up
            Q_EMIT this->wakeup(m_currentCookie);
            this->clearWakeup(m_currentCookie);
        });
        m_timerThread->start();

        qDebug() << "PowerDevil not found, using wait worker thread for time tracking.";
    }
}

int Utilities::scheduleWakeup(quint64 timestamp)
{
    if (this->hasPowerDevil()) {
        QDBusReply<uint> reply = m_interface->call(QStringLiteral("scheduleWakeup"), QStringLiteral("org.kde.kclockd"), QDBusObjectPath("/Utility"), timestamp);
        m_cookies.append(reply.value());
        return reply.value();
    } else {
        m_list.append({++m_cookie, timestamp});
        this->schedule();
        return m_cookie;
    }
}

void Utilities::clearWakeup(int cookie)
{
    if (this->hasPowerDevil()) {
        auto index = m_cookies.indexOf(cookie);
        if (index != -1) {
            m_interface->call(QStringLiteral("clearWakeup"), cookie);
            m_cookies.removeAt(index);
        }
    } else {
        auto index = 0;
        for (auto tuple : m_list) {
            if (cookie == std::get<0>(tuple)) {
                break;
            }
            ++index;
        }
        m_list.removeAt(index);

        this->schedule();
    }
}

void Utilities::wakeupCallback(int cookie)
{
    qDebug() << "wakeup callback";
    auto index = m_cookies.indexOf(cookie);

    if (index == -1) {
        // something must be wrong here, return and do nothing
        qDebug() << "callback ignored (wrong cookie)";
        return;
    } else {
        // remove token
        m_cookies.removeAt(index);
        Q_EMIT wakeup(cookie);
    }
}

void Utilities::schedule()
{
    auto minTime = std::numeric_limits<long long>::max();

    for (auto tuple : m_list) {
        if (minTime > std::get<1>(tuple)) {
            minTime = std::get<1>(tuple);
            m_currentCookie = std::get<0>(tuple);
        }
    }
    m_worker->setNewTime(minTime); // Unix uses int64 internally for time, if we don't have anything to wait, we wait to year 2262 A.D.
}
