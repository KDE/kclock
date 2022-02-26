/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 * Copyright 2022 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

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
    Q_PROPERTY(QString commandTimeout READ commandTimeout WRITE setCommandTimeout NOTIFY commandTimeoutChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(bool looping READ looping NOTIFY loopingChanged)

public:
    explicit Timer(int length = 0, QString label = QString{}, QString commandTimeout = QString{}, bool running = false, QObject *parent = nullptr);
    explicit Timer(const QJsonObject &obj, QObject *parent);

    ~Timer();

    void init();

    // serialize this timer to json
    QJsonObject serialize();

    Q_SCRIPTABLE void toggleRunning();
    Q_SCRIPTABLE void toggleLooping();
    Q_SCRIPTABLE void reset();
    Q_SCRIPTABLE int elapsed() const;
    Q_SCRIPTABLE QString getUUID();

    const QUuid &uuid() const;

    const int &length() const;
    void setLength(int length);

    const QString &label() const;
    void setLabel(QString label);

    const QString &commandTimeout() const;
    void setCommandTimeout(QString commandTimeout);

    const bool &looping() const;
    const bool &running() const;

Q_SIGNALS:
    Q_SCRIPTABLE void lengthChanged();
    Q_SCRIPTABLE void labelChanged();
    Q_SCRIPTABLE void runningChanged();
    Q_SCRIPTABLE void loopingChanged();
    Q_SCRIPTABLE void commandTimeoutChanged();

private Q_SLOTS:
    void timeUp(int cookie);
    void reschedule();

private:
    void setRunning(bool running);
    void sendNotification();

    // -- properties persisted to storage: --

    // the uuid of the timer
    QUuid m_uuid;

    // the total length of the timer, in seconds
    int m_length;

    // the name of the timer, can be blank
    QString m_label;

    // the command to run when the timer finishes, can be blank
    QString m_commandTimeout;

    // -- properties that are not persisted: --

    // the unix timestamp (seconds) at which the timer was started
    int m_startTime = 0;

    // the time the timer elapsed till the most recent pause/stop, only updated when timer is stopped or finished
    int m_hasElapsed = 0;

    // the PowerDevil cookie used for system wakeup when the timer is supposed to go off
    int m_cookie = -1;

    // whether the timer is running
    bool m_running = false;

    // whether the timer is looping
    bool m_looping = false;
};
