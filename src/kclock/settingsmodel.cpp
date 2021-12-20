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
    m_timeFormat = m_interface->timeFormat();

    connect(m_interface, &LocalKClockSettingsInterface::timeFormatChanged, this, [this]() {
        QString timeFormat = m_interface->timeFormat();

        if (timeFormat != m_timeFormat) {
            m_timeFormat = timeFormat;
            Q_EMIT timeFormatChanged();
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
