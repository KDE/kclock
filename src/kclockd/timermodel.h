/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KCLOCK_TIMERMODEL_H
#define KCLOCK_TIMERMODEL_H

#include <QObject>

class Timer;

class TimerModel : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kclock.TimerModel")

public:
    static TimerModel *instance()
    {
        static TimerModel *singleton = new TimerModel();
        return singleton;
    }

    void load();
    void save();
    Q_SCRIPTABLE void addTimer(int length, QString label, bool running);
    Q_SCRIPTABLE void removeTimer(QString uuid);
Q_SIGNALS:
    Q_SCRIPTABLE void timerAdded(QString);
    Q_SCRIPTABLE void timerRemoved(QString);

private:
    void remove(int index);

    explicit TimerModel();

    QList<Timer *> m_timerList;
};

#endif // KCLOCK_TIMERMODEL_H
