/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KCLOCKD_TIMER_H
#define KCLOCKD_TIMER_H

#include "timermodel.h"

#include <QDateTime>
#include <QObject>
#include <QUuid>

class Timer : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kclock.Timer")
    Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(bool looping READ looping NOTIFY loopingChanged)
    Q_PROPERTY(bool isCommandTimeout READ isCommandTimeout NOTIFY isCommandTimeoutChanged)
    Q_PROPERTY(QString commandTimeout READ commandTimeout NOTIFY commandTimeoutChanged)

public:
    explicit Timer(int length = 0, QString label = QStringLiteral(), bool running = false);
    explicit Timer(const QJsonObject &obj);
    ~Timer();

    QJsonObject serialize();

    Q_SCRIPTABLE void toggleRunning();
    Q_SCRIPTABLE void toggleLooping();
    Q_SCRIPTABLE void toggleIsCommandTimeout();
    Q_SCRIPTABLE void saveCommandTimeout(QString);
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
    const bool &looping() const
    {
        return m_looping;
    }
    const bool &isCommandTimeout() const
    {
        return m_isCommandTimeout;
    }
    const QString &commandTimeout() const
    {
        return m_commandTimeout;
    }

Q_SIGNALS:
    Q_SCRIPTABLE void lengthChanged();
    Q_SCRIPTABLE void labelChanged();
    Q_SCRIPTABLE void runningChanged();
    Q_SCRIPTABLE void loopingChanged();
    Q_SCRIPTABLE void isCommandTimeoutChanged();
    Q_SCRIPTABLE void commandTimeoutChanged();
private Q_SLOTS:
    void timeUp(int cookie);
    void reschedule();

private:
    void setRunning(bool running);
    void sendNotification();

    QUuid m_uuid;
    int m_length, m_startTime = 0; // seconds
    int m_hasElapsed = 0; // time has elapsed till stop, only be updated if stopped or finished
    int m_cookie = -1;
    QString m_label;
    bool m_running = false;
    bool m_looping = false;
    bool m_isCommandTimeout = false;
    QString m_commandTimeout = "";
};

#endif // KCLOCKD_TIMER_H
