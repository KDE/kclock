/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "timerinterface.h"

#include <QObject>
#include <QTimer>
#include <QUuid>

class Timer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(QString lengthPretty READ lengthPretty NOTIFY lengthChanged)
    Q_PROPERTY(int elapsed READ elapsed NOTIFY elapsedChanged)
    Q_PROPERTY(QString elapsedPretty READ elapsedPretty NOTIFY elapsedChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(QString commandTimeout READ commandTimeout WRITE setCommandTimeout NOTIFY commandTimeoutChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(bool looping READ looping NOTIFY loopingChanged)
    Q_PROPERTY(bool ringing READ ringing NOTIFY ringingChanged)

public:
    explicit Timer(QString uuid = QString{}, QObject *parent = nullptr);

    const QUuid &uuid();

    int length() const;
    QString lengthPretty() const;
    void setLength(int length);

    int elapsed() const;
    QString elapsedPretty() const;

    QString label() const;
    void setLabel(QString label);

    QString commandTimeout() const;
    void setCommandTimeout(QString commandTimeout);

    bool running() const;

    bool looping() const;

    bool ringing() const;

    Q_INVOKABLE void toggleRunning();
    Q_INVOKABLE void toggleLooping();
    Q_INVOKABLE void reset();
    Q_INVOKABLE void addMinute();
    Q_INVOKABLE void dismiss();

Q_SIGNALS:
    void lengthChanged();
    void elapsedChanged();
    void labelChanged();
    void commandTimeoutChanged();
    void runningChanged();
    void loopingChanged();
    void ringingChanged();

private Q_SLOTS:
    void updateLength();
    void updateElapsed();
    void updateLabel();
    void updateCommandTimeout();
    void updateRunning();
    void updateLooping();
    void updateRinging();

private:
    void animation(bool start);

    int m_length;
    int m_elapsed;

    QString m_label;
    QString m_commandTimeout;

    bool m_running;
    bool m_looping;
    bool m_ringing;

    QUuid m_uuid;

    org::kde::kclock::Timer *m_interface;

    QTimer *m_animationTimer;
};
