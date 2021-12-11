/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "settingsmodel.h"

#include <KLocalizedString>

#include <QDebug>

SettingsModel::SettingsModel()
    : m_interface(new LocalKClockSettingsInterface(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Settings"), QDBusConnection::sessionBus()))
{
    m_volume = m_interface->alarmVolume();

    connect(m_interface, &LocalKClockSettingsInterface::alarmVolumeChanged, this, &SettingsModel::updateVolume);
}

SettingsModel &SettingsModel::instance()
{
    static SettingsModel singleton;
    return singleton;
};

const int &SettingsModel::volume() const
{
    return m_volume;
}

void SettingsModel::setVolume(int volume)
{
    m_interface->setProperty("alarmVolume", volume);
}

void SettingsModel::updateVolume()
{
    m_volume = m_interface->alarmVolume();
    Q_EMIT volumeChanged();
}
