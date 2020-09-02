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

#ifndef KIRIGAMICLOCK_TIMERMODEL_H
#define KIRIGAMICLOCK_TIMERMODEL_H

#include <QObject>
#include <QString>
#include <QTimer>

class Timer;

class TimerModel : public QObject
{
    Q_OBJECT

public:
    static TimerModel *instance()
    {
        static TimerModel* singleton = new TimerModel();
        return singleton;
    }

    void requestSave();
    void updateTimerLoop();
    void updateTimerStatus(); // update qtimer to be on and off depending on if it is needed (for performance)
    bool areTimersInactive(); // if all timers are inactive

    void load();
    void save();
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void move(int oldIndex, int newIndex);
    Q_INVOKABLE int count();
    Q_INVOKABLE Timer *get(int index);

private:
    explicit TimerModel(QObject *parent = nullptr);
    QList<Timer *> timerList;
    QTimer *timer;
    QTimer *saveTimer;
};

#endif // KIRIGAMICLOCK_TIMERMODEL_H
