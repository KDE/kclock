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

    Q_PROPERTY(QString tzName READ getCurrentTimeZoneName NOTIFY propertyChanged)

public:
    static UtilModel *instance();

    Q_INVOKABLE QString getDefaultAlarmFileLocation();

    QString getCurrentTimeZoneName();

    long long calculateNextRingTime(int hours, int minutes, int daysOfWeek, int snooze = 0);
    QString timeToRingFormatted(const long long &timestamp);

    bool isLocale24HourTime();

    void setSelectedTimezone(QByteArray id, bool selected);

private:
    UtilModel(){}; // explicitly declare it private
    bool m_applicationLoaded = false;

Q_SIGNALS:
    void selectedTimezoneChanged(QByteArray id, bool selected);
    void propertyChanged();
    void applicationLoadedChanged();
};
