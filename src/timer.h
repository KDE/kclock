/*
 * Copyright 2020   Han Young <hanyoung@protonmail.com>
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

#include <QDateTime>
#include <QObject>
#include <QUuid>

#include "timermodel.h"

class Timer : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kclock.Timer")
    Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)

public:
    explicit Timer(int length = 0, QString label = QStringLiteral(), bool running = false);
    explicit Timer(const QJsonObject &obj);
    ~Timer();

    QJsonObject serialize();

    Q_SCRIPTABLE void toggleRunning();
    Q_SCRIPTABLE void reset();
    Q_SCRIPTABLE int elapsed() const
    {
        if (running())
            return QDateTime::currentSecsSinceEpoch() - m_startTime;
        else
            return m_hasElapsed;
    }
    Q_SCRIPTABLE QString getUUID()
    {
        return m_uuid.toString();
    }
    const QUuid &uuid() const
    {
        return m_uuid;
    };
    const int &length() const
    {
        return m_length;
    }
    void setLength(int length)
    {
        m_length = length;
        Q_EMIT lengthChanged();
        TimerModel::instance()->save();
    }
    const QString &label() const
    {
        return m_label;
    }
    void setLabel(QString label)
    {
        m_label = label;
        Q_EMIT labelChanged();
        TimerModel::instance()->save();
    }
    const bool &running() const
    {
        return m_running;
    }

Q_SIGNALS:
    Q_SCRIPTABLE void lengthChanged();
    Q_SCRIPTABLE void labelChanged();
    Q_SCRIPTABLE void runningChanged();
private Q_SLOTS:
    void timeUp(int cookie);

private:
    void setRunning(bool running);
    void sendNotification();

    QUuid m_uuid;
    int m_length, m_startTime = 0; // seconds
    int m_hasElapsed = 0;          // time has elapsed till stop, only be updated if stopped or finished
    int m_cookie = -1;
    QString m_label;
    bool m_running = false;
};
