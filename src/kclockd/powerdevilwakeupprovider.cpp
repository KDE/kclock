/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "powerdevilwakeupprovider.h"
#include "utilities.h"

#include <QDBusReply>
#include <QDebug>

PowerDevilWakeupProvider::PowerDevilWakeupProvider(QObject *parent)
    : AbstractWakeupProvider(parent)
    , m_interface(new QDBusInterface(POWERDEVIL_SERVICE_NAME,
                                     QStringLiteral("/org/kde/Solid/PowerManagement"),
                                     QStringLiteral("org.kde.Solid.PowerManagement"),
                                     QDBusConnection::sessionBus(),
                                     this))
{
    QDBusConnection::sessionBus().connect(POWERDEVIL_SERVICE_NAME,
                                          QStringLiteral("/org/kde/Solid/PowerManagement"),
                                          QStringLiteral("org.kde.Solid.PowerManagement"),
                                          QStringLiteral("wakeupCallback"),
                                          this,
                                          SLOT(wakeupCallback(int)));
}

int PowerDevilWakeupProvider::scheduleWakeup(quint64 timestamp)
{
    QDBusReply<uint> reply = m_interface->call(QStringLiteral("scheduleWakeup"), QStringLiteral("org.kde.kclockd"), QDBusObjectPath("/Utility"), timestamp);
    if (reply.isValid()) {
        m_powerDevilCookies.append(reply.value());
    } else {
        qDebug() << "Invalid reply, error: " << reply.error();
    }
    return reply.value();
}

void PowerDevilWakeupProvider::clearWakeup(int cookie)
{
    auto index = m_powerDevilCookies.indexOf(cookie);
    if (index != -1) {
        m_interface->call(QStringLiteral("clearWakeup"), cookie);
        m_powerDevilCookies.removeAt(index);
    }
}

void PowerDevilWakeupProvider::wakeupCallback(int cookie)
{
    qDebug() << "PowerDevilWakeupProvider::wakeupCallback called with cookie:" << cookie;
    auto index = m_powerDevilCookies.indexOf(cookie);

    if (index == -1) {
        // something must be wrong here, return and do nothing
        qDebug() << "Callback ignored (wrong cookie).";
    } else {
        // remove token
        m_powerDevilCookies.removeAt(index);
        Q_EMIT wakeup(cookie);
    }
}

#include "moc_powerdevilwakeupprovider.cpp"
