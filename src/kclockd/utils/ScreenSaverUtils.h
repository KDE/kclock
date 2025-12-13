/*
 * Copyright 2025 Tushar Gupta <tushar.197712@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QObject>

class ScreenSaverUtils
{
    Q_GADGET

public:
    Q_INVOKABLE static bool getActive();
};

Q_DECLARE_METATYPE(ScreenSaverUtils)
