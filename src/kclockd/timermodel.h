/*
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QDBusServiceWatcher>
#include <QHash>
#include <QObject>
#include <QTimer>
#include <QUrl>

class Timer;
class UnityLauncher;

class KNotification;

class TimerModel : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kclock.TimerModel")
    Q_PROPERTY(QString defaultAudioLocation READ defaultAudioLocation WRITE setDefaultAudioLocation NOTIFY defaultAudioLocationChanged)

public:
    static TimerModel *instance();

    void load();
    void save();
    QList<Timer *> timerList() const;
    Timer *timer(const QString &uuid) const;
    Q_SCRIPTABLE QStringList timers() const;
    Q_SCRIPTABLE void addTimer(int length, const QString &label, bool looping, const QString &commandTimeout, bool running);
    Q_SCRIPTABLE void removeTimer(const QString &uuid);

    QString defaultAudioLocation() const;
    void setDefaultAudioLocation(QString location);

Q_SIGNALS:
    Q_SCRIPTABLE void timerAdded(const QString &);
    Q_SCRIPTABLE void timerRemoved(const QString &);
    Q_SCRIPTABLE void defaultAudioLocationChanged(const QString &);

private:
    void connectTimer(Timer *timer);
    void remove(int index);
    void timerRunningChanged();
    void updateIndicators();

    void maybeCreateNotification(Timer *timer);
    void updateNotification(Timer *timer);
    void sendOrClearAllNotifications();

    void onServiceRegistered(const QString &service);
    void onServiceUnregistered(const QString &service);
    void onTimerNotificationSettingChanged();

    static void launchKClock(const QString &xdgActivationToken);

    explicit TimerModel();

    QList<Timer *> m_timerList;

    QDBusServiceWatcher m_kclockWatcher;
    QUrl m_defaultAudioLocation;
    bool m_kclockRunning = false;

    UnityLauncher *m_unityLauncher;
    QHash<Timer *, KNotification *> m_notifications;
    QTimer m_updateIndicatorsTimer;
};
