/*
    SPDX-FileCopyrightText: 2020 HanY <hanyoung@protonmail.com>
    SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kclock_1x2.h"

#include <KLocalizedString>

#include <QDBusConnection>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>

KClock_1x2::KClock_1x2(QObject *parent, const KPluginMetaData &metaData, const QVariantList &args)
    : Plasma::Applet(parent, metaData, args)
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

    QDBusMessage message = QDBusMessage::createMethodCall(QStringLiteral("org.kde.kclockd"),
                                                          QStringLiteral("/Alarms"),
                                                          QStringLiteral("org.kde.kclock.AlarmModel"),
                                                          QStringLiteral("getNextAlarm"));

    QDBusPendingCall call = QDBusConnection::sessionBus().asyncCall(message);
    auto *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher] {
        QDBusPendingReply<quint64> reply = *watcher;

        if (reply.isError()) {
            qWarning() << "Failed to fetch next alarm" << reply.error().name() << reply.error().message();
        } else {
            const auto alarmTime = reply.value();
            if (alarmTime > 0) {
                auto dateTime = QDateTime::fromSecsSinceEpoch(alarmTime).toLocalTime();
                m_string = QStringLiteral("%1, %2").arg(m_locale.standaloneDayName(dateTime.date().dayOfWeek(), QLocale::ShortFormat),
                                                        m_locale.toString(dateTime.time(), QStringLiteral("hh:mm")));
                Q_EMIT alarmTimeChanged();
                m_hasAlarm = true;
                Q_EMIT hasAlarmChanged();
            }
        }

        watcher->deleteLater();
    });
}

QString KClock_1x2::alarmTime() const
{
    return m_string;
}

bool KClock_1x2::hasAlarm() const
{
    return m_hasAlarm;
}

void KClock_1x2::updateAlarm(qulonglong time)
{
    auto dateTime = QDateTime::fromSecsSinceEpoch(time).toLocalTime();
    const bool hasAlarm = time > 0;
    if (hasAlarm) {
        m_string = QStringLiteral("%1, %2").arg(m_locale.standaloneDayName(dateTime.date().dayOfWeek(), QLocale::ShortFormat),
                                                m_locale.toString(dateTime.time(), QStringLiteral("hh:mm")));
        Q_EMIT alarmTimeChanged();
    }

    if (m_hasAlarm != hasAlarm) {
        m_hasAlarm = hasAlarm;
        Q_EMIT hasAlarmChanged();
    }
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

QString KClock_1x2::date() const
{
    return m_locale.toString(QDate::currentDate(), QStringLiteral("ddd, MMM d"));
}

QDateTime KClock_1x2::datetime() const
{
    return QDateTime::currentDateTime();
}

KClock_1x2::~KClock_1x2()
{
}

K_PLUGIN_CLASS(KClock_1x2)

#include "kclock_1x2.moc"

#include "moc_kclock_1x2.cpp"
