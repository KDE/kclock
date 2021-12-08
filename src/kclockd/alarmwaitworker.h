/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QObject>

class AlarmWaitWorker : public QObject
{
    Q_OBJECT

public:
    explicit AlarmWaitWorker(quint64 timestamp = 0);

    void setNewTime(quint64 timestamp); // set new wait time, if is currently waiting

Q_SIGNALS:
    void finished();
    void error();
    void startWait(int waitId);

protected Q_SLOTS:
    void wait(int waitId);

private:
    int m_timerFd;
    int m_waitId; // use only newest poll
    quint64 m_waitEndTime;
    bool m_isFinished = true;
};
