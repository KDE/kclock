/*
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QObject>

class Timer;

class TimerModel : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kclock.TimerModel")

public:
    static TimerModel *instance();

    void load();
    void save();
    Q_SCRIPTABLE QStringList timers() const;
    Q_SCRIPTABLE void addTimer(int length, QString label, QString commandTimeout, bool running);
    Q_SCRIPTABLE void removeTimer(QString uuid);

Q_SIGNALS:
    Q_SCRIPTABLE void timerAdded(QString);
    Q_SCRIPTABLE void timerRemoved(QString);

private:
    void remove(int index);

    explicit TimerModel();

    QList<Timer *> m_timerList;
};
