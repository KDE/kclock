#include <KLocalizedString>
#include <KNotification>

#include <QJsonObject>

#include "timer.h"
#include "utilities.h"
/* ~ Timer ~ */

Timer::Timer(int length, QString label, bool running)
    : m_length(length)
    , m_label(label)
    , m_running(running)
{
    connect(&Utilities::instance(), &Utilities::wakeup, this, &Timer::timeUp);
}

Timer::Timer(const QJsonObject &obj)
{
    m_length = obj[QStringLiteral("length")].toInt();
    m_label = obj[QStringLiteral("label")].toString();

    connect(&Utilities::instance(), &Utilities::wakeup, this, &Timer::timeUp);
}

QJsonObject Timer::serialize()
{
    QJsonObject obj;
    obj[QStringLiteral("length")] = QString::number(m_length);
    obj[QStringLiteral("label")] = m_label;
    return obj;
}

void Timer::toggleRunning()
{
    if (!m_running) {
        setRunning(true);
    } else {
        setRunning(false);
    }

    Q_EMIT runningChanged();
}

void Timer::reset()
{
    setRunning(false);
    m_hasElapsed = 0;

    Q_EMIT runningChanged();
}

void Timer::timeUp(int cookie)
{
    if (cookie == m_cookie) {
        this->sendNotification();
        this->m_cookie = -1;
        this->setRunning(false);
    }
}

void Timer::setRunning(bool running)
{
    if (m_running == running)
        return;

    if (m_running) {
        m_hasElapsed = QDateTime::currentSecsSinceEpoch() - m_startTime;
        if (m_cookie > 0) {
            Utilities::instance().unregiser(m_cookie);
            m_cookie = -1;
        }
    } else {
        m_startTime = QDateTime::currentSecsSinceEpoch();
        m_cookie = Utilities::instance().regiser(m_startTime + m_length);
    }

    m_running = running;

    Q_EMIT runningChanged();
}

void Timer::sendNotification()
{
    qDebug("Timer finished, sending notification...");

    KNotification *notif = new KNotification("timerFinished");
    notif->setIconName("kclock");
    notif->setTitle(i18n("Timer complete"));
    notif->setText(i18n("Your timer has finished!"));
    notif->setDefaultAction(i18n("View"));
    notif->setUrgency(KNotification::HighUrgency);
    notif->setFlags(KNotification::NotificationFlag::LoopSound | KNotification::NotificationFlag::Persistent);
    notif->sendEvent();
}
