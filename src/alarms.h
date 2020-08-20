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

#ifndef KIRIGAMICLOCK_ALARMS_H
#define KIRIGAMICLOCK_ALARMS_H

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
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY propertyChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY propertyChanged)
    Q_PROPERTY(int hours READ hours WRITE setHours NOTIFY propertyChanged)
    Q_PROPERTY(int minutes READ minutes WRITE setMinutes NOTIFY propertyChanged)
    Q_PROPERTY(int daysOfWeek READ daysOfWeek WRITE setDaysOfWeek NOTIFY propertyChanged)
    Q_PROPERTY(int snoozedMinutes READ snoozedMinutes NOTIFY propertyChanged)
    Q_PROPERTY(QString ringtoneName READ ringtoneName WRITE setRingtoneName NOTIFY propertyChanged)
    Q_PROPERTY(QString ringtonePath READ ringtonePath WRITE setRingtonePath NOTIFY propertyChanged)

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
        Q_EMIT propertyChanged();
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
            this->m_snooze = 0;         // reset snooze value
            this->m_nextRingTime = -1; // reset next ring time

            this->m_enabled = enabled;
            emit alarmChanged(); // notify the AlarmModel to reschedule
            Q_EMIT propertyChanged();
        }
    }
    int hours() const
    {
        return m_hours;
    }
    void setHours(int hours)
    {
        this->m_hours = hours;
        Q_EMIT propertyChanged();
    }
    int minutes() const
    {
        return m_minutes;
    }
    void setMinutes(int minutes)
    {
        this->m_minutes = minutes;
        Q_EMIT propertyChanged();
    }
    int daysOfWeek() const
    {
        return m_daysOfWeek;
    }
    void setDaysOfWeek(int daysOfWeek)
    {
        this->m_daysOfWeek = daysOfWeek;
        Q_EMIT propertyChanged();
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
        Q_EMIT propertyChanged();
    }
    inline QString ringtoneName()
    {
        return m_ringtoneName;
    };
    inline void setRingtoneName(QString name)
    {
        m_ringtoneName = name;
        emit propertyChanged();
    };
    void setRingtone(QString urlStr)
    {
        auto url = QUrl(urlStr);
        m_audioPath = url;
    };
    QString ringtonePath()
    {
        return m_audioPath.toString();
    };
    void setRingtonePath(QString path)
    {
        m_audioPath = path;
        emit propertyChanged();
    }
    QString serialize();

    qint64 nextRingTime(); // the next time this should ring, if this would never ring, return -1
    void ring();           // ring alarm

signals:
    void propertyChanged();
    Q_SCRIPTABLE void alarmChanged();
public slots:
    Q_SCRIPTABLE QString getUUID()
    {
        return m_uuid.toString();
    }
private slots:
    void save(); // serialize and save to config
private:
    void calculateNextRingTime();

    bool alarmNotifOpen = false; // if the alarm notification is open
    QTime alarmNotifOpenTime;    // time the alarm notification opened

    QUrl m_audioPath = QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "sounds/freedesktop/stereo/alarm-clock-elapsed.oga"));
    QString m_name = "New Alarm";
    QString m_ringtoneName = "default";
    QUuid m_uuid;
    bool m_enabled;
    bool m_justSnoozed = false; // pressing snooze on the notification also triggers the dismiss event, so this is a helper for that
    int m_hours = 0, m_minutes = 0, m_daysOfWeek = 0;
    qint64 m_snooze = 0;         // current snooze length
    qint64 m_nextRingTime = -1; // store calculated next ring time
};

#endif // KIRIGAMICLOCK_ALARMS_H
