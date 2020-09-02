#include <KLocalizedString>
#include <KNotification>

#include <QJsonObject>

#include "timer.h"
#include "timermodel.h"
/* ~ Timer ~ */

Timer::Timer(QObject *parent, int length, int elapsed, QString label, bool running)
    : QObject(parent)
    , m_length(length)
    , m_elapsed(elapsed)
    , m_label(label)
    , m_running(running)
    , m_finished(false)
    , m_justCreated(true)
{
    connect(
        this, &Timer::propertyChanged, this, [] { TimerModel::instance()->requestSave(); }, Qt::UniqueConnection);
}

Timer::Timer(const QJsonObject &obj)
{
    m_length = obj[QStringLiteral("length")].toInt();
    m_elapsed = obj[QStringLiteral("elapsed")].toInt();
    m_label = obj[QStringLiteral("label")].toString();
    m_running = obj[QStringLiteral("running")].toBool();
    m_finished = obj[QStringLiteral("finished")].toBool();
    m_justCreated = false;

    connect(
        this, &Timer::propertyChanged, this, [] { TimerModel::instance()->requestSave(); }, Qt::UniqueConnection);
}

QJsonObject Timer::serialize()
{
    QJsonObject obj;
    obj[QStringLiteral("length")] = QString::number(m_length);
    obj[QStringLiteral("elapsed")] = QString::number(m_elapsed);
    obj[QStringLiteral("label")] = m_label;
    obj[QStringLiteral("running")] = m_running;
    obj[QStringLiteral("finished")] = m_finished;
    return obj;
}

void Timer::updateTimer(qint64 duration)
{
    if (m_running) {
        m_elapsed += duration;

        // if the timer has finished
        if (m_elapsed >= m_length) {
            m_elapsed = m_length;
            m_running = false;
            m_finished = true;

            qDebug("Timer finished, sending notification...");

            KNotification *notif = new KNotification("timerFinished");
            notif->setIconName("kclock");
            notif->setTitle(i18n("Timer complete"));
            notif->setText(i18n("Your timer has finished!"));
            notif->setDefaultAction(i18n("View"));
            notif->setUrgency(KNotification::HighUrgency);
            notif->setFlags(KNotification::NotificationFlag::LoopSound | KNotification::NotificationFlag::Persistent);
            notif->sendEvent();

            TimerModel::instance()->updateTimerStatus();
        }

        emit propertyChanged();
    }
}

void Timer::toggleRunning()
{
    if (m_finished) {
        m_elapsed = 0;
        m_running = true;
        m_finished = false;
    } else {
        m_running = !m_running;
    }

    TimerModel::instance()->updateTimerStatus();

    emit propertyChanged();
}

void Timer::reset()
{
    m_finished = false;
    m_running = false;
    m_elapsed = 0;

    TimerModel::instance()->updateTimerStatus();

    emit propertyChanged();
}
