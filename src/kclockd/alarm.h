/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "alarmmodel.h"
#include "alarmplayer.h"
#include "alarmwaitworker.h"

#include <QDebug>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QObject>
#include <QStandardPaths>
#include <QString>
#include <QThread>
#include <QTime>
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
    Q_PROPERTY(int snoozedMinutes READ snoozedMinutes NOTIFY snoozedMinutesChanged)
    Q_PROPERTY(QString ringtonePath READ ringtonePath WRITE setRingtonePath NOTIFY ringtonePathChanged)

public Q_SLOTS:
    void handleDismiss();
    void handleSnooze();
    void save(); // serialize and save to config

public:
    static void bumpRingingCount();
    static void lowerRingingCount();
    static int ringing();

    explicit Alarm(AlarmModel *parent = nullptr, QString name = QStringLiteral(""), int minutes = 0, int hours = 0, int daysOfWeek = 0);
    explicit Alarm(QString serialized, AlarmModel *parent = nullptr);

    QString name() const;
    void setName(QString name);

    QUuid uuid() const;

    bool enabled() const;
    void setEnabled(bool enabled);

    int hours() const;
    void setHours(int hours);

    int minutes() const;
    void setMinutes(int minutes);

    int daysOfWeek() const;
    void setDaysOfWeek(int daysOfWeek);

    int snoozedMinutes() const;
    int snooze() const;
    void setSnooze(int snooze);

    QString ringtonePath() const;
    void setRingtonePath(QString path);
    QString serialize();

    void ring(); // ring alarm
    Q_SCRIPTABLE quint64 nextRingTime(); // the next time this should ring, if this would never ring, return -1
    Q_SCRIPTABLE QString getUUID();

Q_SIGNALS:
    void nameChanged();
    void enabledChanged();
    void hoursChanged();
    void minutesChanged();
    void daysOfWeekChanged();
    void snoozedMinutesChanged();
    void ringtonePathChanged();

    Q_SCRIPTABLE void propertyChanged(QString property);
    Q_SCRIPTABLE void alarmChanged();

private:
    void initialize(AlarmModel *parent); // called after object construction
    void calculateNextRingTime();

    QUrl m_audioPath =
        QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("sounds/freedesktop/stereo/alarm-clock-elapsed.oga")));
    QString m_name;
    QUuid m_uuid;
    bool m_enabled = true;
    bool m_justSnoozed = false; // pressing snooze on the notification also triggers the dismiss event, so this is a helper for that
    int m_hours = 0, m_minutes = 0, m_daysOfWeek = 0;
    int m_snooze = 0; // current snooze length
    quint64 m_nextRingTime = 0; // store calculated next ring time

    bool alarmNotifOpen = false; // if the alarm notification is open
    QTime alarmNotifOpenTime; // time the alarm notification opened

    static std::atomic<int> ringingCount;
};
