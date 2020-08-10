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
#include <QMediaPlayer>
#include <QObject>
#include <QStandardPaths>
#include <QString>
#include <QTime>
#include <QUrl>
#include <QUuid>

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
    Q_PROPERTY(QString ringtoneName READ ringtoneName WRITE setRingtoneName NOTIFY propertyChanged)
    Q_PROPERTY(QString ringtonePath WRITE setRingtone NOTIFY propertyChanged)
    Q_PROPERTY(qreal volume READ volume WRITE setVolume)

public slots:
    void handleDismiss();
    void handleSnooze();

public:
    explicit Alarm(AlarmModel *parent = nullptr, QString name = "", int minutes = 0, int hours = 0, int daysOfWeek = 0);
    explicit Alarm(QString serialized, AlarmModel *parent = nullptr);
    ~Alarm();
    QString name() const
    {
        return name_;
    }
    void setName(QString name)
    {
        this->name_ = name;
        Q_EMIT propertyChanged();
    }
    QUuid uuid() const
    {
        return uuid_;
    }
    bool enabled() const
    {
        return enabled_;
    }
    void setEnabled(bool enabled)
    {
        this->enabled_ = enabled;
        Q_EMIT propertyChanged();
    }
    int hours() const
    {
        return hours_;
    }
    void setHours(int hours)
    {
        this->hours_ = hours;
        Q_EMIT propertyChanged();
    }
    int minutes() const
    {
        return minutes_;
    }
    void setMinutes(int minutes)
    {
        this->minutes_ = minutes;
        Q_EMIT propertyChanged();
    }
    int daysOfWeek() const
    {
        return daysOfWeek_;
    }
    void setDaysOfWeek(int daysOfWeek)
    {
        this->daysOfWeek_ = daysOfWeek;
        Q_EMIT propertyChanged();
    }
    qint64 lastAlarm() const
    {
        return lastAlarm_;
    }
    void setLastAlarm(qint64 lastAlarm)
    {
        this->lastAlarm_ = lastAlarm;
        Q_EMIT propertyChanged();
    }
    qint64 snooze() const
    {
        return snooze_;
    }
    void setSnooze(qint64 snooze)
    {
        this->snooze_ = snooze;
        Q_EMIT propertyChanged();
    }
    qint64 lastSnooze() const
    {
        return lastSnooze_;
    }
    void setLastSnooze(qint64 lastSnooze)
    {
        this->lastSnooze_ = lastSnooze;
        Q_EMIT propertyChanged();
    }
    inline QString ringtoneName()
    {
        return ringtoneName_;
    };
    inline void setRingtoneName(QString name)
    {
        ringtoneName_ = name;
        emit propertyChanged();
    };
    void setRingtone(QString urlStr)
    {
        auto url = QUrl(urlStr);
        audioPath_ = url;

        ringtonePlayer->setMedia(audioPath_);
        ringtonePlayer->play();
        Q_EMIT propertyChanged();
    };

    Q_INVOKABLE void play()
    {
        ringtonePlayer->play();
    }; // for testing volume
    Q_INVOKABLE void stop()
    {
        ringtonePlayer->stop();
    }; // stop volume testing when user done with alarm setting
    Q_INVOKABLE QString selectRingtone()
    {
        return QFileDialog::getOpenFileUrl(nullptr, tr("Select Ringtone"), QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), tr("Audio Files (*.wav *.mp3 *.opus *.aac *.ogg)")).toString();
    };
    QString serialize();

    qint64 nextRingTime();   // the next time this should ring, if this would never ring, return -1
    void ring();             // ring alarm
    Q_INVOKABLE void save(); // serialize and save to config

    inline qreal volume()
    {
        return volume_;
    };
    inline void setVolume(qreal volume)
    {
        volume_ = volume;
        ringtonePlayer->setVolume(volume);
    };
    void loopAlarmSound(QMediaPlayer::State state); // called when alarm sound ends (whether or not to play it again)

signals:
    void propertyChanged();
    Q_SCRIPTABLE void alarmChanged();

private:
    QMediaPlayer *ringtonePlayer;
    bool alarmNotifOpen = false; // if the alarm notification is open
    QTime alarmNotifOpenTime;    // time the alarm notification opened

    QUrl audioPath_ = QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "sounds/freedesktop/stereo/alarm-clock-elapsed.oga"));
    qreal volume_ = 100;
    QString name_ = "New Alarm";
    QString ringtoneName_ = "default";
    QUuid uuid_;
    bool enabled_;
    int hours_ = 0, minutes_ = 0, daysOfWeek_ = 0;
    qint64 snooze_ = 0;     // current snooze length
    qint64 lastSnooze_ = 0; // last snooze length (cache snooze_ since it is set to 0 when alarm rings)
};

#endif // KIRIGAMICLOCK_ALARMS_H
