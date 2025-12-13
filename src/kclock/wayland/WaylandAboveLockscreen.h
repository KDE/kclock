/*
 * Copyright 2025 Tushar Gupta <tushar.197712@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#ifndef WAYLANDABOVELOCKSCREEN_H
#define WAYLANDABOVELOCKSCREEN_H

#include <QGuiApplication>
#include <QWaylandClientExtensionTemplate>
#include <QWindow>
#include <QtGui/qpa/qplatformnativeinterface.h>

#ifdef KCLOCK_BUILD_SHELL_OVERLAY
#include "qwayland-kde-lockscreen-overlay-v1.h"

class WaylandAboveLockscreen : public QWaylandClientExtensionTemplate<WaylandAboveLockscreen>, public QtWayland::kde_lockscreen_overlay_v1
{
public:
    WaylandAboveLockscreen();
    void allowWindow(QWindow *window);
    void raiseWindow(QWindow *window);
};

#endif // KCLOCK_BUILD_SHELL_OVERLAY

#endif // WAYLANDABOVELOCKSCREEN_H
