/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "abstractwakeupprovider.h"

#include <QDBusInterface>
#include <QList>

class PowerDevilWakeupProvider : public AbstractWakeupProvider
{
    Q_OBJECT

public:
    explicit PowerDevilWakeupProvider(QObject *parent = nullptr);

    int scheduleWakeup(quint64 timestamp) override;
    void clearWakeup(int cookie) override;

public Q_SLOTS:
    Q_SCRIPTABLE void wakeupCallback(int cookie);

private:
    QDBusInterface *m_interface = nullptr;
    QList<int> m_powerDevilCookies; // token for PowerDevil: https://invent.kde.org/plasma/powerdevil/-/merge_requests/13
};
