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
#include <QtCore/QUuid>
#include <QtCore/QAbstractListModel>

class Alarm : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY onPropertyChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY onPropertyChanged)
    Q_PROPERTY(int hours READ getHours WRITE setHours NOTIFY onPropertyChanged)
    Q_PROPERTY(int minutes READ getMinutes WRITE setMinutes NOTIFY onPropertyChanged)
    Q_PROPERTY(QString dayOfWeek READ getDayOfWeek WRITE setDayOfWeek NOTIFY onPropertyChanged)

public:
    explicit Alarm(QObject *parent = nullptr, QString name = "", int minutes = 0, int hours = 0, QString dayOfWeek = "*");

    QString getName() const { return name; }
    QUuid getUuid() const { return uuid; }
    bool isEnabled() const { return enabled; }
    bool isDirty() const { return dirty; }
    int getHours() const { return cronHours; }
    int getMinutes() const { return cronMinutes; }
    QString getDayOfWeek() const { return cronDayOfWeek; }
    QString getCronString();
    
    void setName(QString name) { this->name = name; }
    void setEnabled(bool enabled) { this->enabled = enabled; }
    void setHours(int hours) { this->cronHours = hours; }
    void setMinutes(int minutes) { this->cronMinutes = minutes; }
    void setDayOfWeek(QString dayOfWeek) { this->cronDayOfWeek = dayOfWeek; }

signals:
    void onPropertyChanged();

private:
    QString name, cronDayOfWeek;
    QUuid uuid;
    bool enabled, dirty = false;
    int cronHours, cronMinutes;
};

class AlarmModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AlarmModel(QObject *parent = nullptr) : QAbstractListModel (parent) { }

    enum {
        EnabledRole = Qt::UserRole + 1,
        HoursRole = Qt::DisplayRole + 0,
        MinutesRole = Qt::DisplayRole + 1,
        NameRole = Qt::DisplayRole + 2,
    };

    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex & index) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString getCrontabUpdate(const QString crontab);
    bool load();

    Q_INVOKABLE bool save();
    Q_INVOKABLE Alarm* insert(int index, QString name, int minutes, int hours, QString dayOfWeek);
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE Alarm* get(int index);

private:
    QList<Alarm*> alarmsList;
};

#endif //KIRIGAMICLOCK_ALARMS_H
