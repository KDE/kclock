/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QObject>
#include <QString>

class UtilModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString tzName READ getCurrentTimeZoneName CONSTANT)
    Q_PROPERTY(bool use24HourTime READ use24HourTime NOTIFY use24HourTimeChanged)

public:
    UtilModel(QObject *parent = nullptr);
    static UtilModel *instance();

    Q_INVOKABLE QString getDefaultAlarmFileLocation();

    QString getCurrentTimeZoneName();

    long long calculateNextRingTime(int hours, int minutes, int daysOfWeek, int snooze = 0);
    QString timeToRingFormatted(const long long &timestamp);

    QString timeFormat() const;
    bool use24HourTime() const;

private:
    bool isLocale24HourTime() const;

Q_SIGNALS:
    void use24HourTimeChanged();
};
