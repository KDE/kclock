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
    Q_PROPERTY(QString timeFormat READ timeFormat WRITE setTimeFormat NOTIFY timeFormatChanged)

public:
    static SettingsModel *instance();

    QString timeFormat() const;
    void setTimeFormat(QString timeFormat);

Q_SIGNALS:
    void timeFormatChanged();

private:
    SettingsModel();
    LocalKClockSettingsInterface *m_interface;
    QSettings m_settings;

    QString m_timeFormat;
};
