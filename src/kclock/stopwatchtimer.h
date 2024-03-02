// SPDX-FileCopyrightText: 2020 Han Young <hanyoung@protonmail.com>
// SPDX-FileCopyrightText: 2020-2024 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QDateTime>
#include <QObject>
#include <QTimer>

// class for the timer that manages the stopwatch
class StopwatchTimer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool paused READ paused NOTIFY pausedChanged)
    Q_PROPERTY(bool stopped READ stopped NOTIFY stoppedChanged)
    Q_PROPERTY(qint64 elapsedTime READ elapsedTime NOTIFY timeChanged)
    Q_PROPERTY(QString hours READ hoursDisplay NOTIFY timeChanged)
    Q_PROPERTY(QString minutes READ minutesDisplay NOTIFY timeChanged)
    Q_PROPERTY(QString seconds READ secondsDisplay NOTIFY timeChanged)
    Q_PROPERTY(QString small READ smallDisplay NOTIFY timeChanged)

public:
    static StopwatchTimer *instance();
    explicit StopwatchTimer(QObject *parent = nullptr);

    bool paused();
    bool stopped();

    qint64 hours() const;
    qint64 minutes() const;
    qint64 seconds() const;
    qint64 small() const;
    QString hoursDisplay() const;
    QString minutesDisplay() const;
    QString secondsDisplay() const;
    QString smallDisplay() const;

    qint64 elapsedTime() const;

    Q_INVOKABLE void reset();
    Q_INVOKABLE void toggle();

Q_SIGNALS:
    void pausedChanged();
    void stoppedChanged();
    void timeChanged();
    void resetTriggered();

private:
    // when the stopwatch started
    qint64 m_timerStartStamp = 0;

    // when the last pause started
    qint64 m_pausedStamp = 0;

    // how much total time has been spent paused since the stopwatch started
    qint64 m_pausedElapsed = 0;

    // whether the stopwatch is fully stopped (laps cleared, start at 0)
    bool m_stopped = true;

    // whether the stopwatch is stopped or paused
    bool m_paused = false;

    QTimer *m_timer = nullptr;
};
