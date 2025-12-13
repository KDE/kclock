/*
 * Copyright 2025 Tushar Gupta <tushar.197712@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "wayland/WaylandAboveLockscreen.h"

#include <KWaylandExtras>
#include <KWindowSystem>
#include <QFuture>
#include <QObject>
#include <QString>
void allowAboveLockscreen(QWindow *window, WaylandAboveLockscreen *overlay)
{
    if (KWindowSystem::isPlatformWayland()) {
        Q_ASSERT(!window->isVisible());
        if (!overlay->isInitialized()) {
            qWarning() << "Compositor does NOT support kde-lockscreen-overlay-v1. Skipping above-lockscreen feature.";
            return;
        }
        overlay->allowWindow(window);
    } else {
        qDebug() << Q_FUNC_INFO << "Alarm shell overlay is supported only for Wayland";
    }
}

void raiseWindow(QWindow *window)
{
#ifdef KCLOCK_BUILD_SHELL_OVERLAY

    if (KWindowSystem::isPlatformWayland()) {
        window->setVisibility(QWindow::Visibility::FullScreen);
        QFuture<QString> token = KWaylandExtras::xdgActivationToken(window, QStringLiteral("org.kde.kclock.desktop"));
        token.then([window](const QString &token) {
            KWindowSystem::setCurrentXdgActivationToken(token);
            KWindowSystem::activateWindow(window);
        });
    } else {
        qDebug() << Q_FUNC_INFO << "Screen is locked. Alarm shell overlay is supported only for Wayland";
    }
#else // KCLOCK_BUILD_SHELL_OVERLAY
    window->raise();
#endif // KCLOCK_BUILD_SHELL_OVERLAY
}
