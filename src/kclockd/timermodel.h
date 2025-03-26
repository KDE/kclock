/*
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QObject>
#include <QTimer>

class Timer;
class UnityLauncher;

class TimerModel : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kclock.TimerModel")

public:
    static TimerModel *instance();

    void load();
    void save();
    QList<Timer *> timerList() const;
    Timer *timer(const QString &uuid) const;
    Q_SCRIPTABLE QStringList timers() const;
    Q_SCRIPTABLE void addTimer(int length, const QString &label, const QString &commandTimeout, bool running);
    Q_SCRIPTABLE void removeTimer(const QString &uuid);

Q_SIGNALS:
    Q_SCRIPTABLE void timerAdded(const QString &);
    Q_SCRIPTABLE void timerRemoved(const QString &);

private:
    void connectTimer(Timer *timer);
    void remove(int index);
    void updateUnityLauncher();

    explicit TimerModel();

    QList<Timer *> m_timerList;
    UnityLauncher *m_unityLauncher;
    QTimer m_updateUnityLauncherTimer;
};
