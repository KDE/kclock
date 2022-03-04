/*
 * Copyright 2022 Devin Lin <devin@kde.org>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "xdgportal.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDebug>

#include <KLocalizedString>

void XDGPortal::requestBackground()
{
    qDebug() << "Requesting portal to run in the background and autostart...";

    // create our initial request
    QDBusMessage autostartMsg = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.portal.Desktop"),
                                                               QStringLiteral("/org/freedesktop/portal/desktop"),
                                                               QStringLiteral("org.freedesktop.portal.Background"),
                                                               QStringLiteral("RequestBackground"));

    // build arguments
    QList<QVariant> msgArgs;
    msgArgs << QString(); // parent_window - we leave blank since this is a daemon

    QMap<QString, QVariant> msgOpts = {
        {QStringLiteral("handle_token"), m_handleToken},
        {QStringLiteral("reason"), i18n("Allow the clock process to be in the background and launched on startup.")},
        {QStringLiteral("autostart"), true},
        {QStringLiteral("dbus-activatable"), true},
    };
    msgArgs << msgOpts;

    autostartMsg.setArguments(msgArgs);

    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(autostartMsg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall);

    // listen to response
    connect(watcher, &QDBusPendingCallWatcher::finished, [this](QDBusPendingCallWatcher *watcher) {
        QDBusPendingReply<QDBusObjectPath> reply = *watcher;

        if (reply.isError()) {
            qWarning() << "Could not get reply from org.freedesktop.portal.Background:" << reply.error().message();
        } else {
            // get response object from org.freedesktop.portal.Request
            QDBusConnection::sessionBus().connect(QString(),
                                                  reply.value().path(),
                                                  QLatin1String("org.freedesktop.portal.Request"),
                                                  QLatin1String("Response"),
                                                  this,
                                                  SLOT(requestBackgroundResponse(uint, QVariantMap)));
        }
    });
}

void XDGPortal::requestBackgroundResponse(uint response, const QVariantMap &results)
{
    if (!response) {
        qDebug() << "Request for autostart with org.freedesktop.portal.Background results:";
        for (auto key : results.keys()) {
            qDebug() << "  " << key << ":" << results[key].toString();
        }

        // stop interaction
        QDBusMessage stopMsg = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.portal.Request"),
                                                              QStringLiteral("/org/freedesktop/portal/Request"),
                                                              QStringLiteral("org.freedesktop.portal.Request"),
                                                              QStringLiteral("Close"));

        QDBusConnection::sessionBus().call(stopMsg);
    } else {
        qWarning() << "Failed to receive response from org.freedesktop.portal.Request.";
    }
}
