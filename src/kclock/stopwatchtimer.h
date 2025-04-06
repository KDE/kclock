// SPDX-FileCopyrightText: 2020 Han Young <hanyoung@protonmail.com>
// SPDX-FileCopyrightText: 2020-2024 Devin Lin <devin@kde.org>
// SPDX-FileCopyrightText: 2025 Kai Uwe Broulik <kde@broulik.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <QTimer>

#include <optional>

#include <qqmlintegration.h>

class QJSEngine;
class QQmlEngine;

// class for the timer that manages the stopwatch
class StopwatchTimer : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool paused READ paused NOTIFY pausedChanged)
    Q_PROPERTY(bool stopped READ stopped NOTIFY stoppedChanged)
    Q_PROPERTY(qint64 elapsedTime READ elapsedTime NOTIFY timeChanged)
    Q_PROPERTY(QString hours READ hoursDisplay NOTIFY timeChanged)
    Q_PROPERTY(QString minutes READ minutesDisplay NOTIFY timeChanged)
    Q_PROPERTY(QString seconds READ secondsDisplay NOTIFY timeChanged)
    Q_PROPERTY(QString small READ smallDisplay NOTIFY timeChanged)

public:
    static StopwatchTimer *instance();
    static StopwatchTimer *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    bool paused() const;
    bool stopped() const;

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
    explicit StopwatchTimer(QObject *parent = nullptr);

    QElapsedTimer m_elapsedTimer;

    std::optional<qint64> m_pausedTime;

    QTimer m_reportTimer;
};
