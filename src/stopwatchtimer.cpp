#include "stopwatchtimer.h"
#include <QTimer>
#include <QDebug>

StopwatchTimer::StopwatchTimer(QObject *parent)
    : QObject(parent)
{
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &StopwatchTimer::updateTime);
}

void StopwatchTimer::updateTime()
{
    emit timeChanged();
}

void StopwatchTimer::toggle()
{
    if (stopped) { // start (from zero)
        stopped = false;
        paused = false;
        
        timerStartStamp = QDateTime::currentMSecsSinceEpoch();
        pausedElapsed = 0;
        
        timer_->start(interval_);
    } else if (paused) { // unpause
        paused = false;
        
        pausedElapsed += QDateTime::currentMSecsSinceEpoch() - pausedStamp;
        
        timer_->start(interval_);
    } else { // pause
        paused = true;
        pausedStamp = QDateTime::currentMSecsSinceEpoch();
        timer_->stop();
    }
}

void StopwatchTimer::reset()
{
    timer_->stop();
    pausedElapsed = 0;
    stopped = true;
    paused = false;
    emit timeChanged();
}


long long StopwatchTimer::elapsedTime()
{
    long long cur = QDateTime::currentMSecsSinceEpoch();
    if (stopped) {
        return 0;
    } else if (paused) {
        return cur - timerStartStamp - pausedElapsed - (cur - pausedStamp);
    } else {
        return cur - timerStartStamp - pausedElapsed;
    }
}

long long StopwatchTimer::minutes()
{
    return elapsedTime() / 1000 / 60;
}

long long StopwatchTimer::seconds()
{
    return elapsedTime() / 1000 - 60 * minutes();
}

long long StopwatchTimer::small()
{
    return elapsedTime() / 10 - 100 * seconds() - 100 * 60 * minutes();
}

QString StopwatchTimer::minutesDisplay()
{
    long long amount = minutes();
    return amount >= 10 ? QString::number(amount) : "0" + QString::number(amount);
}

QString StopwatchTimer::secondsDisplay()
{
    long long amount = seconds();
    return amount >= 10 ? QString::number(amount) : "0" + QString::number(amount);
}

QString StopwatchTimer::smallDisplay()
{
    long long amount = small();
    return amount >= 10 ? QString::number(amount) : "0" + QString::number(amount);
}
