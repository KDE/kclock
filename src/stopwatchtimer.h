#ifndef STOPWATCHTIMER_H
#define STOPWATCHTIMER_H

#include <QDateTime>
#include <QObject>

class QTimer;
class StopwatchTimer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int elapsedTime READ elapsedTime NOTIFY timeChanged)
    Q_PROPERTY(QString minutes READ minutesDisplay NOTIFY timeChanged)
    Q_PROPERTY(QString seconds READ secondsDisplay NOTIFY timeChanged)
    Q_PROPERTY(QString small READ smallDisplay NOTIFY timeChanged)

public:
    explicit StopwatchTimer(QObject *parent = nullptr);

    long long minutes();
    long long seconds();
    long long small();
    QString minutesDisplay();
    QString secondsDisplay();
    QString smallDisplay();

    long long elapsedTime();

    Q_INVOKABLE void reset();
    Q_INVOKABLE void toggle();

signals:
    void timeChanged();

private slots:
    void updateTime();

private:
    const int interval_ = 41; // 24fps

    long long timerStartStamp = QDateTime::currentMSecsSinceEpoch();
    long long pausedStamp = QDateTime::currentMSecsSinceEpoch();
    long long pausedElapsed = 0;

    bool stopped = true, paused = false;
    QTimer *timer_;
};

#endif // STOPWATCHTIMER_H
