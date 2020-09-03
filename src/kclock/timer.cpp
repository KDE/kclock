#include <QTimer>

#include "timer.h"
/* ~ Timer ~ */
Timer::Timer()
{
}
Timer::Timer(QString uuid, bool justCreated)
    : m_interface(new OrgKdeKclockTimerInterface(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Timers/") + uuid, QDBusConnection::sessionBus(), this))
    , m_justCreated(justCreated)
{
    if (m_interface->isValid()) {
        m_uuid = QUuid(m_interface->getUUID());
        m_label = m_interface->label();
        m_length = m_interface->length();
        m_running = m_interface->running();
        m_elapsed = m_interface->elapsed();
        connect(m_interface, &OrgKdeKclockTimerInterface::lengthChanged, this, &Timer::updateLength);
        connect(m_interface, &OrgKdeKclockTimerInterface::labelChanged, this, &Timer::updateLabel);
        connect(m_interface, &OrgKdeKclockTimerInterface::runningChanged, this, &Timer::updateRunning);
    }
}

void Timer::updateLength()
{
    m_length = m_interface->length();
    Q_EMIT propertyChanged();
}

void Timer::updateLabel()
{
    m_label = m_interface->label();
    Q_EMIT propertyChanged();
}
void Timer::updateRunning()
{
    m_running = m_interface->running();
    Q_EMIT propertyChanged();
    if (m_running) {
        this->animation(true);
    } else {
        this->animation(false);
    }
}

void Timer::animation(bool start)
{
    if (!m_timer) {
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, [this] {
            m_elapsed = m_interface->elapsed();
            Q_EMIT this->elapsedChanged();
        });
    }

    if (start) {
        m_timer->start(1000);
    } else {
        m_timer->stop();
    }
}
