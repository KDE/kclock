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

#include <QObject>
#include <QString>
#include <QtCore/QAbstractListModel>
#include <QtCore/QUuid>
#include <QTimer>

class Alarm : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY onPropertyChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY onPropertyChanged)
    Q_PROPERTY(int hours READ getHours WRITE setHours NOTIFY onPropertyChanged)
    Q_PROPERTY(int minutes READ getMinutes WRITE setMinutes NOTIFY onPropertyChanged)
    Q_PROPERTY(int dayOfWeek READ getDayOfWeek WRITE setDayOfWeek NOTIFY onPropertyChanged)

public:
    explicit Alarm(QObject *parent = nullptr, QString name = "", int minutes = 0, int hours = 0, int dayOfWeek = 0);
    explicit Alarm(QString serialized);

    QString getName() const
    {
        return name;
    }
    QUuid getUuid() const
    {
        return uuid;
    }
    bool isEnabled() const
    {
        return enabled;
    }
    int getHours() const
    {
        return hours;
    }
    int getMinutes() const
    {
        return minutes;
    }
    int getDayOfWeek() const
    {
        return dayOfWeek;
    }
    qint64 getLastAlarm() const
    {
        return lastAlarm;
    }

    void setName(QString name)
    {
        this->name = name;
    }
    void setEnabled(bool enabled)
    {
        this->enabled = enabled;
    }
    void setHours(int hours)
    {
        this->hours = hours;
    }
    void setMinutes(int minutes)
    {
        this->minutes = minutes;
    }
    void setDayOfWeek(int dayOfWeek)
    {
        this->dayOfWeek = dayOfWeek;
    }
    void setLastAlarm(qint64 lastAlarm)
    {
        this->lastAlarm = lastAlarm;
    }

    QString serialize();
    qint64 toPreviousAlarm(qint64 timestamp); // the last alarm (timestamp) that should have played
    void ring(); // ring alarm
    void save(); // serialize and save to config
    
signals:
    void onPropertyChanged();

private:
    QString name;
    QUuid uuid;
    bool enabled;
    int hours, minutes, dayOfWeek;
    qint64 lastAlarm; // last time the alarm ran (unix timestamp)
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
        DayOfWeekRole = Qt::DisplayRole + 3,
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void updateUi();

    bool load();
    void checkAlarmsToRun();

    Q_INVOKABLE Alarm *insert(int index, QString name, int minutes, int hours, int dayOfWeek);
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE Alarm *get(int index);

private:
    QList<Alarm *> alarmsList;
    
    QTimer* timer;
};

#endif // KIRIGAMICLOCK_ALARMS_H
