/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *                Han Young <hanyoung@protonmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <QDebug>
#include <QFileDialog>
#include <QObject>
#include <QStandardPaths>
#include <QString>
#include <QTime>
#include <QUrl>
#include <QUuid>

#include "alarmplayer.h"
class QMediaPlayer;
class QThread;
class AlarmWaitWorker;
class AlarmModel;
const QString ALARM_CFG_GROUP = "Alarms";
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

public slots:
    void handleDismiss();
    void handleSnooze();

public:
    explicit Alarm(AlarmModel *parent = nullptr, QString name = "", int minutes = 0, int hours = 0, int daysOfWeek = 0);
    explicit Alarm(QString serialized, AlarmModel *parent = nullptr);
    QString name() const
    {
        return m_name;
    }
    void setName(QString name)
    {
        this->m_name = name;
        Q_EMIT nameChanged();
        Q_EMIT propertyChanged("name");
    }
    QUuid uuid() const
    {
        return m_uuid;
    }
    bool enabled() const
    {
        return m_enabled;
    }
    void setEnabled(bool enabled)
    {
        if (this->m_enabled != enabled) {
            this->m_snooze = 0;        // reset snooze value
            this->m_nextRingTime = -1; // reset next ring time

            this->m_enabled = enabled;
            Q_EMIT alarmChanged(); // notify the AlarmModel to reschedule
            Q_EMIT enabledChanged();
            Q_EMIT propertyChanged("enabled");
        }
    }
    int hours() const
    {
        return m_hours;
    }
    void setHours(int hours)
    {
        this->m_hours = hours;
        Q_EMIT alarmChanged();
        Q_EMIT hoursChanged();
        Q_EMIT propertyChanged("hours");
    }
    int minutes() const
    {
        return m_minutes;
    }
    void setMinutes(int minutes)
    {
        this->m_minutes = minutes;
        Q_EMIT alarmChanged();
        Q_EMIT minutesChanged();
        Q_EMIT propertyChanged("minutes");
    }
    int daysOfWeek() const
    {
        return m_daysOfWeek;
    }
    void setDaysOfWeek(int daysOfWeek)
    {
        this->m_daysOfWeek = daysOfWeek;
        Q_EMIT alarmChanged();
        Q_EMIT daysOfWeekChanged();
        Q_EMIT propertyChanged("daysOfWeek");
    }
    int snoozedMinutes() const
    {
        if (m_snooze != 0 && m_enabled) {
            return m_snooze / 60;
        } else {
            return 0;
        }
    }
    qint64 snooze() const
    {
        return m_snooze;
    }
    void setSnooze(qint64 snooze)
    {
        this->m_snooze = snooze;
        Q_EMIT snoozedMinutesChanged();
        Q_EMIT propertyChanged("snoozedMinutes");
    }
    QString ringtonePath() const
    {
        return m_audioPath.toString();
    };
    void setRingtonePath(QString path)
    {
        m_audioPath = QUrl(path);
        Q_EMIT ringtonePathChanged();
        Q_EMIT propertyChanged("ringtonePath");
    }
    QString serialize();

    Q_SCRIPTABLE qint64 nextRingTime(); // the next time this should ring, if this would never ring, return -1
    void ring();                        // ring alarm
    Q_SCRIPTABLE QString getUUID()
    {
        return m_uuid.toString();
    }
signals:
    void nameChanged();
    void enabledChanged();
    void hoursChanged();
    void minutesChanged();
    void daysOfWeekChanged();
    void snoozedMinutesChanged();
    void ringtonePathChanged();
    Q_SCRIPTABLE void propertyChanged(QString property);
    Q_SCRIPTABLE void alarmChanged();
private slots:
    void save(); // serialize and save to config
private:
    void calculateNextRingTime();

    bool alarmNotifOpen = false; // if the alarm notification is open
    QTime alarmNotifOpenTime;    // time the alarm notification opened

    QUrl m_audioPath = QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "sounds/freedesktop/stereo/alarm-clock-elapsed.oga"));
    QString m_name = "New Alarm";
    QUuid m_uuid;
    bool m_enabled = true;
    bool m_justSnoozed = false; // pressing snooze on the notification also triggers the dismiss event, so this is a helper for that
    int m_hours = 0, m_minutes = 0, m_daysOfWeek = 0;
    qint64 m_snooze = 0;        // current snooze length
    qint64 m_nextRingTime = -1; // store calculated next ring time
};
