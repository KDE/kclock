/*
    SPDX-FileCopyrightText: 2020 HanY <hanyoung@protonmail.com>
    SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kclock_1x2.h"

#include <KLocalizedString>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QProcess>
#include <QTimer>

KClock_1x2::KClock_1x2(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &KClock_1x2::initialTimeUpdate);
    m_timer->setSingleShot(true);

    // initial interval is milliseconds to next minute
    m_timer->start((60 - (QTime::currentTime().msecsSinceStartOfDay() / 1000) % 60) * 1000); // seconds to next minute

    bool daemonSuccess = QDBusConnection::sessionBus().connect(QStringLiteral("org.kde.kclockd"),
                                                               QStringLiteral("/Alarms"),
                                                               QStringLiteral("org.kde.kclock.AlarmModel"),
                                                               QStringLiteral("nextAlarm"),
                                                               this,
                                                               SLOT(updateAlarm(qulonglong)));
    if (!daemonSuccess) {
        m_string = QStringLiteral("connection to kclockd failed");
    }

    QDBusInterface *interface = new QDBusInterface(QStringLiteral("org.kde.kclockd"),
                                                   QStringLiteral("/Alarms"),
                                                   QStringLiteral("org.kde.kclock.AlarmModel"),
                                                   QDBusConnection::sessionBus(),
                                                   this);
    QDBusReply<quint64> reply = interface->call(QStringLiteral("getNextAlarm"));

    if (reply.isValid()) {
        auto alarmTime = reply.value();
        if (alarmTime > 0) {
            auto dateTime = QDateTime::fromSecsSinceEpoch(alarmTime).toLocalTime();
            m_string = m_locale.standaloneDayName(dateTime.date().dayOfWeek(), QLocale::ShortFormat) + QStringLiteral(", ")
                + m_locale.toString(dateTime.time(), QStringLiteral("hh:mm"));
            m_hasAlarm = true;
        } else {
            m_hasAlarm = false;
        }
    }

    Q_EMIT propertyChanged();
}

QString KClock_1x2::alarmTime()
{
    return m_string;
};

bool KClock_1x2::hasAlarm()
{
    return m_hasAlarm;
}

void KClock_1x2::updateAlarm(qulonglong time)
{
    auto dateTime = QDateTime::fromSecsSinceEpoch(time).toLocalTime();
    if (time > 0) {
        m_string = m_locale.standaloneDayName(dateTime.date().dayOfWeek(), QLocale::ShortFormat) + QStringLiteral(", ")
            + m_locale.toString(dateTime.time(), QStringLiteral("hh:mm"));
        m_hasAlarm = true;
    } else {
        m_hasAlarm = false;
    }
    Q_EMIT propertyChanged();
}

void KClock_1x2::openKClock()
{
    m_process = new QProcess(this);
    m_process->start(QStringLiteral("kclock"), QStringList());
}

void KClock_1x2::initialTimeUpdate()
{
    Q_EMIT timeChanged();
    disconnect(m_timer, &QTimer::timeout, this, &KClock_1x2::initialTimeUpdate); // disconnect
    m_timer->setSingleShot(false);
    connect(m_timer, &QTimer::timeout, this, &KClock_1x2::timeChanged);
    m_timer->start(60000); // update every minute
}

QString KClock_1x2::date()
{
    return m_locale.toString(QDate::currentDate(), QStringLiteral("ddd, MMM d"));
}

KClock_1x2::~KClock_1x2()
{
}

K_EXPORT_PLASMA_APPLET_WITH_JSON(kclock_1x2, KClock_1x2, "metadata.json")

#include "kclock_1x2.moc"
