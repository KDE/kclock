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
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString formattedTime READ formattedTime NOTIFY formattedTimeChanged)
    Q_PROPERTY(int hours READ hours WRITE setHours NOTIFY hoursChanged)
    Q_PROPERTY(int minutes READ minutes WRITE setMinutes NOTIFY minutesChanged)
    Q_PROPERTY(int daysOfWeek READ daysOfWeek WRITE setDaysOfWeek NOTIFY daysOfWeekChanged)
    Q_PROPERTY(QString audioPath READ audioPath WRITE setAudioPath NOTIFY audioPathChanged)
    Q_PROPERTY(int ringDuration READ ringDuration WRITE setRingDuration NOTIFY ringDurationChanged)
    Q_PROPERTY(int snoozeDuration READ snoozeDuration WRITE setSnoozeDuration NOTIFY snoozeDurationChanged)
    Q_PROPERTY(int snoozedLength READ snoozedLength NOTIFY snoozedLengthChanged)
    Q_PROPERTY(bool ringing READ ringing NOTIFY ringingChanged)
    Q_PROPERTY(quint64 nextRingTime READ nextRingTime NOTIFY nextRingTimeChanged)

public:
    explicit Alarm();
    explicit Alarm(QString uuid);

    QUuid uuid() const;

    QString name() const;
    void setName(QString name);

    bool enabled() const;
    void setEnabled(bool enabled);

    QString formattedTime() const;

    int hours() const;
    void setHours(int hours);

    int minutes() const;
    void setMinutes(int minutes);

    int daysOfWeek() const;
    void setDaysOfWeek(int daysOfWeek);

    QString audioPath() const;
    void setAudioPath(QString path);

    int ringDuration() const;
    void setRingDuration(int ringDuration);

    int snoozeDuration() const;
    void setSnoozeDuration(int snoozeDuration);

    int snoozedLength() const;

    bool ringing() const;

    quint64 nextRingTime() const;

    bool isValid() const;

    Q_INVOKABLE void dismiss();
    Q_INVOKABLE void snooze();

    Q_INVOKABLE QString timeToRingFormatted();

    Q_INVOKABLE void save();

Q_SIGNALS:
    void nameChanged();
    void enabledChanged();
    void formattedTimeChanged();
    void hoursChanged();
    void minutesChanged();
    void daysOfWeekChanged();
    void audioPathChanged();
    void ringDurationChanged();
    void snoozeDurationChanged();
    void snoozedLengthChanged();
    void ringingChanged();
    void nextRingTimeChanged();

private Q_SLOTS:
    void updateName();
    void updateEnabled();
    void updateHours();
    void updateMinutes();
    void updateDaysOfWeek();
    void updateAudioPath();
    void updateRingDuration();
    void updateSnoozeDuration();
    void updateSnoozedLength();
    void updateRinging();
    void updateNextRingTime();

private:
    org::kde::kclock::Alarm *m_interface;

    QUuid m_uuid;

    QString m_name;
    bool m_enabled;
    int m_hours;
    int m_minutes;
    int m_daysOfWeek;
    QString m_audioPath;
    int m_ringDuration;
    int m_snoozeDuration;
    int m_snoozedLength;
    bool m_ringing;
    qint64 m_nextRingTime;

    bool m_isValid = true;
};
