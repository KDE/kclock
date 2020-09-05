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

#include <QObject>

class AlarmWaitWorker : public QObject
{
    Q_OBJECT
public:
    explicit AlarmWaitWorker(qint64 timestamp = -1);

    void setNewTime(qint64 timestamp); // set new wait time, if is currently waiting
Q_SIGNALS:
    void finished();
    void error();
    void startWait(int waitId);

protected Q_SLOTS:
    void wait(int waitId);

private:
    int m_timerFd;
    int m_waitId; // use only newest poll 
    qint64 m_waitEndTime;
    bool m_isFinished = true;
};
