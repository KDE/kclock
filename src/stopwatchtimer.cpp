#include "stopwatchtimer.h"
#include <QTimer>
#include <QtDebug>
StopwatchTimer::StopwatchTimer(QObject *parent)
    : QObject(parent)
{
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &StopwatchTimer::updateTime);
}

void StopwatchTimer::updateTime()
{
    elapsedTime_ += interval_;
    emit timeChanged();
}

void StopwatchTimer::toggle()
{
    if (timer_->isActive()) {
        timer_->stop();
        qDebug() << "stop";
    } else {
        timer_->start(interval_);
        qDebug() << "start";
    }
}

void StopwatchTimer::reset()
{
    timer_->stop();
    elapsedTime_ = 0;
    emit timeChanged();
}
