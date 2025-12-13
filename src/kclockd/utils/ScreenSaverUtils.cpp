/*
 * Copyright 2025 Tushar Gupta <tushar.197712@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "ScreenSaverUtils.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>

bool ScreenSaverUtils::getActive()
{
    QDBusMessage request = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.ScreenSaver"),
                                                          QStringLiteral("/ScreenSaver"),
                                                          QStringLiteral("org.freedesktop.ScreenSaver"),
                                                          QStringLiteral("GetActive"));

    const QDBusReply<bool> reply = QDBusConnection::sessionBus().call(request);

    return reply.isValid() ? reply.value() : false;
}
