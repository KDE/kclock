/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "settingsmodel.h"

#include <KLocalizedString>

#include <QDebug>

SettingsModel::SettingsModel()
    : m_interface(new LocalKClockSettingsInterface(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Settings"), QDBusConnection::sessionBus()))
{
    if (m_interface->isValid()) {
        m_timeFormat = m_interface->timeFormat();
        m_timerNotification = static_cast<KClockSettings::EnumTimerNotification>(m_interface->timerNotification());
    } else {
        // Use system default format if kclockd isn't here
        m_timeFormat = QStringLiteral("SystemDefault");
    }

    connect(m_interface, &LocalKClockSettingsInterface::timeFormatChanged, this, [this]() {
        QString timeFormat = m_interface->timeFormat();

        if (timeFormat != m_timeFormat) {
            m_timeFormat = timeFormat;
            Q_EMIT timeFormatChanged();
        }
    });

    connect(m_interface, &LocalKClockSettingsInterface::timerNotificationChanged, this, [this] {
        const auto timerNotification = static_cast<KClockSettings::EnumTimerNotification>(m_interface->timerNotification());
        if (m_timerNotification != timerNotification) {
            m_timerNotification = timerNotification;
            Q_EMIT timerNotificationChanged(timerNotification);
        }
    });
}

SettingsModel *SettingsModel::instance()
{
    static SettingsModel *singleton = new SettingsModel();
    return singleton;
}

QString SettingsModel::timeFormat() const
{
    return m_timeFormat;
}

void SettingsModel::setTimeFormat(QString timeFormat)
{
    m_interface->setProperty("timeFormat", timeFormat);
}

KClockSettings::EnumTimerNotification SettingsModel::timerNotification() const
{
    return m_timerNotification;
}

void SettingsModel::setTimerNotification(KClockSettings::EnumTimerNotification timerNotification)
{
    m_interface->setProperty("timerNotification", static_cast<int>(timerNotification));
}

#include "moc_settingsmodel.cpp"
