/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "kclocksettingsinterface.h"

#include <QObject>

class SettingsModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)

public:
    static SettingsModel &instance();

    const int &volume() const;
    void setVolume(int volume);

Q_SIGNALS:
    void volumeChanged();

private Q_SLOTS:
    void updateVolume();

private:
    SettingsModel();
    LocalKClockSettingsInterface *m_interface;

    int m_volume;
};
