/*
    SPDX-FileCopyrightText: 2020 HanY <hanyoung@protonmail.com>
    SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#pragma once

#include <Plasma/Applet>
#include <QLocale>
#include <QProcess>
#include <QTimer>

class KClock : public Plasma::Applet
{
    Q_OBJECT
    Q_PROPERTY(QString alarmTime READ alarmTime NOTIFY alarmTimeChanged)
    Q_PROPERTY(bool hasAlarm READ hasAlarm NOTIFY hasAlarmChanged)

public:
    KClock(QObject *parent, const KPluginMetaData &metaData, const QVariantList &args);
    ~KClock();

    QString alarmTime() const;
    bool hasAlarm() const;
    Q_INVOKABLE void openKClock();

Q_SIGNALS:
    void alarmTimeChanged();
    void hasAlarmChanged();

private Q_SLOTS:
    void updateAlarm(qulonglong time);

private:
    QLocale m_locale = QLocale::system();
    bool m_hasAlarm = false;
    QString m_string;
    QProcess *m_process;
};
