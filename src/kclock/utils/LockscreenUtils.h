/*
 * Copyright 2025 Tushar Gupta <tushar.197712@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "wayland/WaylandAboveLockscreen.h"
#include <QWindow>

#ifdef KCLOCK_BUILD_SHELL_OVERLAY

void allowAboveLockscreen(QWindow *window, WaylandAboveLockscreen *overlay);
void raiseWindow(QWindow *window);

#endif // KCLOCK_BUILD_SHELL_OVERLAY
