#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QThread>

#include "alarmwaitworker.h"
#include "utilities.h"
Utilities::Utilities(QObject *parent)
    : QObject(parent)
    , m_interface(new QDBusInterface(QStringLiteral("org.kde.Solid.PowerManagement"), QStringLiteral("/org/kde/Solid/PowerManagement"), QStringLiteral("org.kde.Solid.PowerManagement"), QDBusConnection::sessionBus(), this))
{
    // if PowerDevil is present rely on PowerDevil to track time, otherwise we do it ourself
    if (m_interface->isValid()) {
        // test Plasma 5.20 PowerDevil schedule wakeup feature
        QDBusMessage m = QDBusMessage::createMethodCall(QStringLiteral("org.kde.Solid.PowerManagement"), QStringLiteral("/org/kde/Solid/PowerManagement"), QStringLiteral("org.freedesktop.DBus.Introspectable"), QStringLiteral("Introspect"));
        QDBusReply<QString> result = QDBusConnection::sessionBus().call(m);

        if (result.isValid() && result.value().indexOf(QStringLiteral("scheduleWakeup")) >= 0) { // have this feature
            m_hasPowerDevil = true;
        }
    }

    if (this->hasPowerDevil()) {
        bool success = QDBusConnection::sessionBus().registerObject(QStringLiteral("/Utility"), QStringLiteral("org.kde.PowerManagement"), this, QDBusConnection::ExportScriptableSlots);
        qDebug() << "PowerDevil found, using it for time tracking. Success:" << success;
    } else {
        m_timerThread = new QThread(this);
        m_worker = new AlarmWaitWorker();
        m_worker->moveToThread(m_timerThread);
        connect(m_worker, &AlarmWaitWorker::finished, [this] {
            // notify time is up
            Q_EMIT this->wakeup(m_currentCookie);
            this->unregiser(m_currentCookie);
        });
        m_timerThread->start();

        qDebug() << "PowerDevil not found, using wait worker thread for time tracking.";
    }
}

int Utilities::regiser(qint64 timestamp)
{
    if (this->hasPowerDevil()) {
        QDBusReply<uint> reply = m_interface->call(QStringLiteral("scheduleWakeup"), QStringLiteral("org.kde.kclockd"), QDBusObjectPath("/Utility"), timestamp);
        m_cookies.append(reply.value());
        return reply.value();
    } else {
        m_list.append({++m_cookie, timestamp});
        this->schedule();
        return m_cookie;
    }
}

void Utilities::unregiser(int cookie)
{
    if (this->hasPowerDevil()) {
        auto index = m_cookies.indexOf(cookie);
        if (index != -1) {
            m_interface->call(QStringLiteral("clearWakeup"), cookie);
            m_cookies.removeAt(index);
        }
    } else {
        auto index = 0;
        for (auto tuple : m_list) {
            if (cookie == std::get<0>(tuple)) {
                break;
            }
            ++index;
        }
        m_list.removeAt(index);

        this->schedule();
    }
}

void Utilities::wakeupCallback(int cookie)
{
    qDebug() << "wakeup callback";
    auto index = m_cookies.indexOf(cookie);

    if (index == -1) {
        // something must be wrong here, return and do nothing
        qDebug() << "callback ignored (wrong cookie)";
        return;
    } else {
        // remove token
        m_cookies.removeAt(index);
        Q_EMIT wakeup(cookie);
    }
}

void Utilities::schedule()
{
    auto minTime = std::numeric_limits<long long>::max();

    for (auto tuple : m_list) {
        if (minTime > std::get<1>(tuple)) {
            minTime = std::get<1>(tuple);
            m_currentCookie = std::get<0>(tuple);
        }
    }
    m_worker->setNewTime(minTime); // Unix uses int64 internally for time, if we don't have anything to wait, we wait to year 2262 A.D.
}
