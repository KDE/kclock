/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
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
#include <QTimer>
#include <QUrl>
#include <QtCore/QAbstractListModel>
#include <QtCore/QUuid>
#include <QMediaPlayer>
#include <QTime>

class QMediaPlayer;
class Alarm : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY onPropertyChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY onPropertyChanged)
    Q_PROPERTY(int hours READ hours WRITE setHours NOTIFY onPropertyChanged)
    Q_PROPERTY(int minutes READ minutes WRITE setMinutes NOTIFY onPropertyChanged)
    Q_PROPERTY(int daysOfWeek READ daysOfWeek WRITE setDaysOfWeek NOTIFY onPropertyChanged)
    Q_PROPERTY(QString ringtoneName READ ringtoneName NOTIFY onPropertyChanged)
    Q_PROPERTY(QString ringtonePath WRITE setRingtone NOTIFY onPropertyChanged)

public slots:
    void handleDismiss();
    void handleSnooze();

public:
    explicit Alarm(QObject *parent = nullptr, QString name = "", int minutes = 0, int hours = 0, int daysOfWeek = 0);
    explicit Alarm(QString serialized);
    ~Alarm();
    QString name() const
    {
        return name_;
    }
    void setName(QString name)
    {
        this->name_ = name;
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
    }
    int hours() const
    {
        return hours_;
    }
    void setHours(int hours)
    {
        this->hours_ = hours;
    }
    int minutes() const
    {
        return minutes_;
    }
    void setMinutes(int minutes)
    {
        this->minutes_ = minutes;
    }
    int daysOfWeek() const
    {
        return daysOfWeek_;
    }
    void setDaysOfWeek(int daysOfWeek)
    {
        this->daysOfWeek_ = daysOfWeek;
    }
    qint64 lastAlarm() const
    {
        return lastAlarm_;
    }
    void setLastAlarm(qint64 lastAlarm)
    {
        this->lastAlarm_ = lastAlarm;
    }
    qint64 snooze() const
    {
        return snooze_;
    }
    void setSnooze(qint64 snooze)
    {
        this->snooze_ = snooze;
    }
    qint64 lastSnooze() const
    {
        return lastSnooze_;
    }
    void setLastSnooze(qint64 lastSnooze)
    {
        this->lastSnooze_ = lastSnooze;
    }
    inline QString ringtoneName()
    {
        return ringtoneName_;
    };
    void setRingtone(QString urlStr)
    {
        auto url = QUrl(urlStr);
        ringtoneName_ = url.fileName();
        audioPath = url;
        
        ringtonePlayer->setMedia(audioPath);
    };
    QString serialize();
    
    qint64 toPreviousAlarm(qint64 timestamp); // the last alarm (timestamp) that should have played
    void ring();                              // ring alarm
    void save();                              // serialize and save to config
    
    void loopAlarmSound(QMediaPlayer::State state); // called when alarm sound ends (whether or not to play it again)

signals:
    void onPropertyChanged();

private:
    QMediaPlayer *ringtonePlayer;
    
    QUrl audioPath = QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "sounds/freedesktop/stereo/alarm-clock-elapsed.oga"));
    bool alarmNotifOpen = false; // if the alarm notification is open
    QTime alarmNotifOpenTime; // time the alarm notification opened
    
    QString name_;
    QString ringtoneName_ = "default";
    QUuid uuid_;
    bool enabled_;
    int hours_, minutes_, daysOfWeek_; 
    qint64 lastAlarm_; // last time the alarm ran (unix timestamp)
    qint64 snooze_; // current snooze length
    qint64 lastSnooze_; // last snooze length (cache snooze_ since it is set to 0 when alarm rings)
};

class AlarmModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AlarmModel(QObject *parent = nullptr);

    enum {
        EnabledRole = Qt::UserRole + 1,
        HoursRole = Qt::DisplayRole + 0,
        MinutesRole = Qt::DisplayRole + 1,
        NameRole = Qt::DisplayRole + 2,
        DaysOfWeekRole = Qt::DisplayRole + 3,
        RingtonePathRole = Qt::DisplayRole + 4,
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void updateUi();

    bool load();
    void checkAlarmsToRun();

    Q_INVOKABLE QUrl selectRingtone()
    {
        return QFileDialog::getOpenFileUrl(nullptr,
                                           tr("Select Ringtone"),
                                           QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                           tr("Audio Files (*.wav *.mp3 *.opus *.aac *.ogg)"));
    };
    Q_INVOKABLE void newAlarm(QString name, int minutes, int hours, int daysOfWeek, QUrl ringtone = QUrl());
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE Alarm *get(int index);

private:
    QList<Alarm *> alarmsList;

    QTimer *timer;
};

#endif // KIRIGAMICLOCK_ALARMS_H
