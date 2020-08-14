/*
    SPDX-FileCopyrightText: 2020 HanY <hanyoung@protonmail.com>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kclock.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QProcess>
#include <QTimer>
#include <klocalizedstring.h>

KClock::KClock(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &KClock::initialTimeUpdate);
    m_timer->setSingleShot(true);
    // initial interval is milliseconds to next minute
    m_timer->start(QTime::currentTime().secsTo(QTime(QTime::currentTime().hour(), QTime::currentTime().minute() + 1)) * 1000);

    if (!QDBusConnection::sessionBus().connect("org.kde.kclock", "/alarms", "org.kde.kclock.AlarmModel", "nextAlarm", this, SLOT(updateAlarm(qulonglong))))
        m_string = "connect failed";

    QDBusInterface *interface = new QDBusInterface("org.kde.kclock", "/alarms", "org.kde.kclock.AlarmModel", QDBusConnection::sessionBus(), this);
    QDBusReply<quint64> reply = interface->call("getNextAlarm");
    if (reply.isValid()) {
        auto alarmTime = reply.value();
        if (alarmTime > 0) {
            auto dateTime = QDateTime::fromSecsSinceEpoch(alarmTime).toLocalTime();
            m_string = dateTime.toString("ddd ") + m_local.toString(dateTime.time(), QLocale::ShortFormat);
            m_hasAlarm = true;
        } else
            m_hasAlarm = false;
    }
    emit propertyChanged();
}

void KClock::updateAlarm(qulonglong time)
{
    auto dateTime = QDateTime::fromSecsSinceEpoch(time).toLocalTime();
    if (time > 0) {
        m_string = dateTime.toString("ddd ") + m_local.toString(dateTime.time(), QLocale::ShortFormat);
        m_hasAlarm = true;
    } else {
        m_hasAlarm = false;
    }
    emit propertyChanged();
}
void KClock::openKClock()
{
    m_process = new QProcess(this);
    m_process->start("kclock", QStringList());
}
void KClock::initialTimeUpdate()
{
    emit timeChanged();
    disconnect(m_timer, &QTimer::timeout, this, &KClock::initialTimeUpdate); // disconnect
    m_timer->setSingleShot(false);
    connect(m_timer, &QTimer::timeout, this, &KClock::timeChanged);
    m_timer->start(60000); // update every minute
}
QString KClock::time()
{
    return m_local.toString(QTime::currentTime(), QLocale::ShortFormat);
}
KClock::~KClock()
{
}

K_EXPORT_PLASMA_APPLET_WITH_JSON(kclock, KClock, "metadata.json")

#include "kclock.moc"
