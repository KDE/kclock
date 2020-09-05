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
#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QUuid>

#include "timerinterface.h"
class QTimer;
class Timer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int length READ length WRITE setLength NOTIFY propertyChanged)
    Q_PROPERTY(int elapsed READ elapsed NOTIFY elapsedChanged)
    Q_PROPERTY(QString lengthPretty READ lengthPretty NOTIFY propertyChanged)
    Q_PROPERTY(QString elapsedPretty READ elapsedPretty NOTIFY elapsedChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY propertyChanged)
    Q_PROPERTY(bool running READ running NOTIFY propertyChanged)
    Q_PROPERTY(bool justCreated MEMBER m_justCreated NOTIFY propertyChanged)

public:
    Timer();
    explicit Timer(QString uuid, bool justCreated = true);
    const QUuid &uuid()
    {
        return m_uuid;
    };
    Q_INVOKABLE void toggleRunning();
    Q_INVOKABLE void reset();

    QString lengthPretty() const
    {
        qint64 len = m_length, hours = len / 60 / 60, minutes = len / 60 - hours * 60, seconds = len - hours * 60 * 60 - minutes * 60;
        return QString::number(hours) + ":" + QString::number(minutes).rightJustified(2, '0') + ":" + QString::number(seconds).rightJustified(2, '0');
    }
    QString elapsedPretty() const
    {
        qint64 len = m_elapsed, hours = len / 60 / 60, minutes = len / 60 - hours * 60, seconds = len - hours * 60 * 60 - minutes * 60;
        return QString::number(hours) + ":" + QString::number(minutes).rightJustified(2, '0') + ":" + QString::number(seconds).rightJustified(2, '0');
    }
    int length() const
    {
        return m_length;
    }
    void setLength(int length)
    {
        m_interface->setLength(length);
    }
    const int &elapsed() const
    {
        return m_elapsed;
    }
    const QString &label() const
    {
        return m_label;
    }
    void setLabel(QString label)
    {
        m_interface->setLabel(label);
    }
    const bool &running() const
    {
        return m_running;
    }

signals:
    void propertyChanged();
    void elapsedChanged();
private slots:
    void updateLength();
    void updateLabel();
    void updateRunning();

private:
    void animation(bool start);

    int m_length, m_elapsed; // seconds
    QString m_label;
    bool m_running;
    bool m_justCreated;

    QUuid m_uuid;
    org::kde::kclock::Timer *m_interface;
    QTimer *m_timer = nullptr;
};

#endif // TIMER_H
