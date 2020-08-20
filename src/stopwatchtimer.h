/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
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

#ifndef STOPWATCHTIMER_H
#define STOPWATCHTIMER_H

#include <QDateTime>
#include <QObject>

class QTimer;
class StopwatchTimer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int elapsedTime READ elapsedTime NOTIFY timeChanged)
    Q_PROPERTY(QString minutes READ minutesDisplay NOTIFY timeChanged)
    Q_PROPERTY(QString seconds READ secondsDisplay NOTIFY timeChanged)
    Q_PROPERTY(QString small READ smallDisplay NOTIFY timeChanged)

public:
    explicit StopwatchTimer(QObject *parent = nullptr);

    long long minutes();
    long long seconds();
    long long small();
    QString minutesDisplay();
    QString secondsDisplay();
    QString smallDisplay();

    long long elapsedTime();

    Q_INVOKABLE void reset();
    Q_INVOKABLE void toggle();

signals:
    void timeChanged();

private slots:
    void updateTime();

private:
    const int m_interval = 41; // 24fps

    long long timerStartStamp = QDateTime::currentMSecsSinceEpoch();
    long long pausedStamp = QDateTime::currentMSecsSinceEpoch();
    long long pausedElapsed = 0;

    bool stopped = true, paused = false;
    QTimer *m_timer;
};

#endif // STOPWATCHTIMER_H
