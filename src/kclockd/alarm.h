/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2022 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "alarmmodel.h"
#include "alarmplayer.h"
#include "alarmwaitworker.h"

#include <KLocalizedString>
#include <KNotification>

#include <QDebug>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QObject>
#include <QStandardPaths>
#include <QString>
#include <QThread>
#include <QTime>
#include <QTimer>
#include <QUrl>
#include <QUuid>

const QString ALARM_CFG_GROUP = QStringLiteral("Alarms");

class AlarmModel;

class Alarm : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kclock.Alarm")
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
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
    explicit Alarm(QString serialized, AlarmModel *parent = nullptr);
    explicit Alarm(QString name = i18n("Alarm"),
                   int hours = 0,
                   int minutes = 0,
                   int daysOfWeek = 0,
                   QString audioPath = QString{},
                   int ringDuration = 5,
                   int snoozeDuration = 5,
                   AlarmModel *parent = nullptr);

    void init(AlarmModel *parent);

    ~Alarm();

    // serialize this alarm to json
    QString serialize();

    Q_SCRIPTABLE QString uuid() const;

    QString name() const;
    void setName(QString name);

    bool enabled() const;
    void setEnabled(bool enabled);

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

    // ring the alarm now
    void ring();

    // get the next time the alarm will ring (unix time), or 0 if it will never ring
    quint64 nextRingTime();

public Q_SLOTS:
    Q_SCRIPTABLE void dismiss();
    Q_SCRIPTABLE void snooze();
    Q_SCRIPTABLE void save(); // serialize and save to config

Q_SIGNALS:
    Q_SCRIPTABLE void nameChanged();
    Q_SCRIPTABLE void enabledChanged();
    Q_SCRIPTABLE void hoursChanged();
    Q_SCRIPTABLE void minutesChanged();
    Q_SCRIPTABLE void daysOfWeekChanged();
    Q_SCRIPTABLE void audioPathChanged();
    Q_SCRIPTABLE void ringDurationChanged();
    Q_SCRIPTABLE void snoozeDurationChanged();

    Q_SCRIPTABLE void snoozedLengthChanged();
    Q_SCRIPTABLE void ringingChanged();
    Q_SCRIPTABLE void nextRingTimeChanged();

    // the time that the alarm is going to ring has changed
    Q_SCRIPTABLE void rescheduleRequested();

private:
    void setSnoozedLength(int snoozedLength);
    void setRinging(bool ringing);
    void setNextRingTime(quint64 nextRingTime);
    void calculateNextRingTime();

    QUuid m_uuid;

    // -- properties that persist to storage: --

    // name of the alarm
    QString m_name;

    // whether the alarm is enabled
    bool m_enabled;

    // hour of the day the alarm rings at
    int m_hours;

    // minute of the hour the alarm rings at
    int m_minutes;

    // bitmask of the days of the week that the alarm rings (if zero, it rings once)
    int m_daysOfWeek;

    // the path to the audio file to ring
    QUrl m_audioPath;

    // the amount of time the alarm rings for, in minutes
    int m_ringDuration;

    // the amount of time the alarm adds when snoozed, in minutes
    int m_snoozeDuration;

    // the amount of time snoozing has added to the current ring, in seconds
    int m_snoozedLength = 0;

    // -- properties only relevant for the current state of the alarm (not persisted): --

    // whether the alarm is ringing
    bool m_ringing = false;

    // cache calculated next ring time (unix time)
    quint64 m_nextRingTime = 0;

    // saves the original ring time (without snoozing applied to it)
    quint64 m_originalRingTime = 0;

    // whether snooze just occurred
    bool m_justSnoozed;

    KNotification *m_notification = new KNotification{QStringLiteral("alarm"), KNotification::NotificationFlag::Persistent, this};

    QTimer *m_ringTimer = new QTimer(this);
};
