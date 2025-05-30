/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "kclocksettingsinterface.h"

#include "kclockdsettings.h"

#include <QObject>

#include <qqmlregistration.h>

class QJSEngine;
class QQmlEngine;

struct KClockSettingsForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(Settings)
    QML_UNCREATABLE("Only used for enums")
    QML_FOREIGN(KClockSettings)
};

class SettingsModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString timeFormat READ timeFormat WRITE setTimeFormat NOTIFY timeFormatChanged)
    Q_PROPERTY(KClockSettings::EnumTimerNotification timerNotification READ timerNotification WRITE setTimerNotification NOTIFY timerNotificationChanged)

public:
    static SettingsModel *instance();
    static SettingsModel *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    QString timeFormat() const;
    void setTimeFormat(QString timeFormat);

    KClockSettings::EnumTimerNotification timerNotification() const;
    void setTimerNotification(KClockSettings::EnumTimerNotification timerNotification);
    Q_SIGNAL void timerNotificationChanged(KClockSettings::EnumTimerNotification TimerNotification);

Q_SIGNALS:
    void timeFormatChanged();

private:
    SettingsModel();
    LocalKClockSettingsInterface *m_interface;
    QSettings m_settings;

    QString m_timeFormat;
    KClockSettings::EnumTimerNotification m_timerNotification = KClockSettings::EnumTimerNotification::WhenKClockNotRunning;
};
