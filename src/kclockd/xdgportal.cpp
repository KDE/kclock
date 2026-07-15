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

XDGPortal::XDGPortal(QObject *parent)
    : QObject(parent)
{
}

void XDGPortal::setAutostart(bool autostart)
{
    m_desiredAutostart = autostart;

    if (m_requestPending || (m_lastRequestedAutostart && *m_lastRequestedAutostart == autostart)) {
        return;
    }

    requestBackground();
}

void XDGPortal::requestBackground()
{
    m_requestPending = true;
    m_requestAutostart = m_desiredAutostart;
    m_lastRequestedAutostart = m_requestAutostart;
    Q_EMIT requestStarted();

    qDebug() << "Requesting portal to run in the background with autostart" << m_requestAutostart;

    // create our initial request
    QDBusMessage autostartMsg = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.portal.Desktop"),
                                                               QStringLiteral("/org/freedesktop/portal/desktop"),
                                                               QStringLiteral("org.freedesktop.portal.Background"),
                                                               QStringLiteral("RequestBackground"));

    // Build arguments and subscribe to the predictable request path before
    // making the call, so a fast portal response cannot race the subscription.
    QList<QVariant> msgArgs;
    msgArgs << QString(); // parent_window - we leave blank since this is a daemon

    const QString handleToken = QStringLiteral("kclock%1").arg(++m_handleToken);
    QString sender = QDBusConnection::sessionBus().baseService().mid(1);
    sender.replace(QLatin1Char('.'), QLatin1Char('_'));
    m_requestPath = QStringLiteral("/org/freedesktop/portal/desktop/request/%1/%2").arg(sender, handleToken);

    QStringList cmdLine = {{QStringLiteral("kclockd")}};
    QMap<QString, QVariant> msgOpts = {{QStringLiteral("handle_token"), handleToken},
                                       {QStringLiteral("reason"),
                                        m_requestAutostart ? i18n("Allow the clock process to be in the background and launched on startup.")
                                                           : i18n("Allow the clock process to run in the background.")},
                                       {QStringLiteral("autostart"), m_requestAutostart},
                                       {QStringLiteral("dbus-activatable"), true},
                                       {QStringLiteral("commandline"), cmdLine}};
    msgArgs << msgOpts;

    autostartMsg.setArguments(msgArgs);

    const bool connected = QDBusConnection::sessionBus().connect(QString(),
                                                                 m_requestPath,
                                                                 QLatin1String("org.freedesktop.portal.Request"),
                                                                 QLatin1String("Response"),
                                                                 this,
                                                                 SLOT(requestBackgroundResponse(uint, QVariantMap)));
    if (!connected) {
        qWarning() << "Could not listen for a response from org.freedesktop.portal.Background.";
        finishRequest();
        return;
    }

    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(autostartMsg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall, this);

    // listen to response
    connect(watcher, &QDBusPendingCallWatcher::finished, [this](QDBusPendingCallWatcher *watcher) {
        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
        watcher->deleteLater();

        if (reply.isError()) {
            qWarning() << "Could not get reply from org.freedesktop.portal.Background:" << reply.error().message();
            finishRequest();
        } else if (reply.value().path() != m_requestPath) {
            qWarning() << "org.freedesktop.portal.Background returned an unexpected request path:" << reply.value().path();
        }
    });
}

void XDGPortal::requestBackgroundResponse(uint response, const QVariantMap &results)
{
    if (!response) {
        qDebug() << "Request to org.freedesktop.portal.Background results:";
        for (auto key : results.keys()) {
            qDebug() << "  " << key << ":" << results[key].toString();
        }
    } else {
        qWarning() << "Failed to receive response from org.freedesktop.portal.Request.";
    }

    finishRequest();
}

void XDGPortal::finishRequest()
{
    QDBusConnection::sessionBus().disconnect(QString(),
                                             m_requestPath,
                                             QLatin1String("org.freedesktop.portal.Request"),
                                             QLatin1String("Response"),
                                             this,
                                             SLOT(requestBackgroundResponse(uint, QVariantMap)));
    m_requestPath.clear();
    m_requestPending = false;

    if (m_desiredAutostart != m_requestAutostart) {
        requestBackground();
    }

    Q_EMIT requestFinished();
}

#include "moc_xdgportal.cpp"
