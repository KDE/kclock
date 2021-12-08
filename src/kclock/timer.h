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
    Q_PROPERTY(int length READ length WRITE setLength NOTIFY propertyChanged)
    Q_PROPERTY(int elapsed READ elapsed NOTIFY elapsedChanged)
    Q_PROPERTY(QString lengthPretty READ lengthPretty NOTIFY propertyChanged)
    Q_PROPERTY(QString elapsedPretty READ elapsedPretty NOTIFY elapsedChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY propertyChanged)
    Q_PROPERTY(QString commandTimeout READ commandTimeout WRITE setCommandTimeout NOTIFY propertyChanged)
    Q_PROPERTY(bool running READ running NOTIFY propertyChanged)
    Q_PROPERTY(bool looping READ looping NOTIFY propertyChanged)
    Q_PROPERTY(bool justCreated MEMBER m_justCreated NOTIFY propertyChanged)

public:
    Timer();
    explicit Timer(QString uuid, bool justCreated = true);

    const QUuid &uuid();
    Q_INVOKABLE void toggleRunning();
    Q_INVOKABLE void toggleLooping();
    Q_INVOKABLE void reset();
    Q_INVOKABLE void addMinute();

    QString lengthPretty() const;
    QString elapsedPretty() const;

    int length() const;
    void setLength(int length);

    const int &elapsed() const;

    const QString &label() const;
    void setLabel(QString label);

    const QString &commandTimeout() const;
    void setCommandTimeout(QString commandTimeout);

    const bool &looping() const;
    const bool &running() const;

Q_SIGNALS:
    void propertyChanged();
    void elapsedChanged();

private Q_SLOTS:
    void updateLength();
    void updateLabel();
    void updateLooping();
    void updateCommandTimeout();
    void updateRunning();

private:
    void animation(bool start);

    int m_length, m_elapsed; // seconds
    QString m_label;
    QString m_commandTimeout;
    bool m_running;
    bool m_looping;
    bool m_justCreated;

    QUuid m_uuid;
    org::kde::kclock::Timer *m_interface;
    QTimer *m_timer = nullptr;
};
