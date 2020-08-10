#ifndef ALARMWAITWORKER_H
#define ALARMWAITWORKER_H

#include <QObject>

class AlarmWaitWorker : public QObject
{
    Q_OBJECT
public:
    explicit AlarmWaitWorker(qint64 timestamp = -1);

    void setNewTime(qint64 timestamp); // set new wait time, if is currently waiting
signals:
    void finished();
    void error();
    void startWait();

protected slots:
    void wait();

private:
    int m_timerFd;
    qint64 m_waitEndTime;
    bool m_isFinished = true;
};

#endif // ALARMWAITWORKER_H
