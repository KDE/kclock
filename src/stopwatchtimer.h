#ifndef STOPWATCHTIMER_H
#define STOPWATCHTIMER_H

#include <QObject>
class QTimer;
class StopwatchTimer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int elapsedTime READ elapsedTime NOTIFY timeChanged)
public:
    explicit StopwatchTimer(QObject *parent = nullptr);
    inline int elapsedTime(){
        return elapsedTime_;
    }
    Q_INVOKABLE void reset();
    Q_INVOKABLE void toggle();
signals:
    void timeChanged();
private slots:
    void updateTime();
private:
    const int interval_ = 10; // 10 millisecond
    int elapsedTime_ = 0;
    QTimer *timer_;
};

#endif // STOPWATCHTIMER_H
