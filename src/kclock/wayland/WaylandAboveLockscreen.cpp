/*
 * Copyright 2025 Tushar Gupta <tushar.197712@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifdef KCLOCK_BUILD_SHELL_OVERLAY

#include "WaylandAboveLockscreen.h"

#include <KWaylandExtras>
#include <KWindowSystem>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>
#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QtWaylandClient/qwaylandclientextension.h>

WaylandAboveLockscreen::WaylandAboveLockscreen()
    : QWaylandClientExtensionTemplate<WaylandAboveLockscreen>(1)
{
    initialize();
}

void WaylandAboveLockscreen::allowWindow(QWindow *window)
{
    QPlatformNativeInterface *native = qGuiApp->platformNativeInterface();
    void *resource = native->nativeResourceForWindow("surface", window);
    wl_surface *surface = reinterpret_cast<wl_surface *>(resource);

    if (!surface) {
        Q_ASSERT(surface);
        return;
    }
    allow(surface);
}

#endif // KCLOCK_BUILD_SHELL_OVERLAY
