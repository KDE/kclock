/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "alarminterface.h"

#include <QFileDialog>
#include <QObject>
#include <QStandardPaths>
#include <QString>
#include <QUrl>
#include <QUuid>

class Alarm : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY propertyChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY propertyChanged)
    Q_PROPERTY(int hours READ hours WRITE setHours NOTIFY propertyChanged)
    Q_PROPERTY(int minutes READ minutes WRITE setMinutes NOTIFY propertyChanged)
    Q_PROPERTY(int daysOfWeek READ daysOfWeek WRITE setDaysOfWeek NOTIFY propertyChanged)
    Q_PROPERTY(int snoozedMinutes READ snoozedMinutes NOTIFY propertyChanged)
    Q_PROPERTY(QString ringtonePath READ ringtonePath WRITE setRingtonePath NOTIFY propertyChanged)

public:
    explicit Alarm();
    explicit Alarm(QString uuid);

    const QString &name() const;
    void setName(QString name);

    const QUuid &uuid() const;

    const bool &enabled() const;
    void setEnabled(bool enabled);

    const int &hours() const;
    void setHours(int hours);

    const int &minutes() const;
    void setMinutes(int minutes);

    const int &daysOfWeek() const;
    void setDaysOfWeek(int daysOfWeek);

    int snoozedMinutes() const;

    const QString &ringtonePath() const;
    void setRingtonePath(QString path);

    bool isValid();

    Q_INVOKABLE QString timeToRingFormated();

    Q_INVOKABLE void save();

Q_SIGNALS:
    void propertyChanged();

private Q_SLOTS:
    void updateProperty(QString property);

private:
    const qint64 &nextRingTime() const;

    void calculateNextRingTime();

    org::kde::kclock::Alarm *m_interface;

    QString m_ringtonePath;
    QString m_name;
    QUuid m_uuid;
    bool m_enabled;
    int m_hours, m_minutes, m_daysOfWeek;
    qint64 m_snooze;
    qint64 m_nextRingTime;

    bool m_isValid = true;
};
