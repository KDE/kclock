/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QObject>

class AbstractWakeupProvider : public QObject
{
    Q_OBJECT

public:
    explicit AbstractWakeupProvider(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
    virtual ~AbstractWakeupProvider() = default;

    virtual int scheduleWakeup(quint64 timestamp) = 0;
    virtual void clearWakeup(int cookie) = 0;

Q_SIGNALS:
    void wakeup(int cookie);
};
