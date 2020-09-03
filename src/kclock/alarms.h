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

#include <QFileDialog>
#include <QObject>
#include <QStandardPaths>
#include <QString>
#include <QUrl>
#include <QUuid>

#include "alarminterface.h"
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
    const QString &name() const
    {
        return m_name;
    }
    void setName(QString name)
    {
        m_interface->setProperty("name", name);
    }
    const QUuid &uuid() const
    {
        return m_uuid;
    }
    const bool &enabled() const
    {
        return m_enabled;
    }
    void setEnabled(bool enabled)
    {
        m_interface->setProperty("enabled", enabled);
        m_enabled = enabled;
    }
    const int &hours() const
    {
        return m_hours;
    }
    void setHours(int hours)
    {
        m_interface->setProperty("hours", hours);
    }
    const int &minutes() const
    {
        return m_minutes;
    }
    void setMinutes(int minutes)
    {
        m_interface->setProperty("minutes", minutes);
    }
    const int &daysOfWeek() const
    {
        return m_daysOfWeek;
    }
    void setDaysOfWeek(int daysOfWeek)
    {
        m_interface->setProperty("daysOfWeek", daysOfWeek);
    }
    int snoozedMinutes() const
    {
        return m_snooze / 60;
    }
    const QString &ringtonePath() const
    {
        return m_ringtonePath;
    };
    void setRingtonePath(QString path)
    {
        m_interface->setProperty("ringtonePath", path);
    }

    bool isValid()
    {
        return m_isValid;
    };

    Q_INVOKABLE QString timeToRingFormated();

    Q_INVOKABLE void save()
    {
        m_interface->alarmChanged();
    };
signals:
    void propertyChanged();
private slots:
    void updateProperty(QString property);

private:
    const qint64 &nextRingTime() const
    {
        return m_nextRingTime;
    };

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

#endif // KIRIGAMICLOCK_ALARMS_H
