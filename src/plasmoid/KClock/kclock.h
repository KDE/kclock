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
    Q_PROPERTY(QDateTime datetime READ datetime NOTIFY timeChanged)
    Q_PROPERTY(QString date READ date NOTIFY timeChanged)
    Q_PROPERTY(QString alarmTime READ alarmTime NOTIFY alarmTimeChanged)
    Q_PROPERTY(bool hasAlarm READ hasAlarm NOTIFY hasAlarmChanged)

public:
    KClock(QObject *parent, const KPluginMetaData &metaData, const QVariantList &args);
    ~KClock();

    QString date() const;
    QDateTime datetime() const;
    QString alarmTime() const;
    bool hasAlarm() const;
    Q_INVOKABLE void openKClock();

Q_SIGNALS:
    void timeChanged();
    void alarmTimeChanged();
    void hasAlarmChanged();

private Q_SLOTS:
    void updateAlarm(qulonglong time);
    void initialTimeUpdate(); // making sure time is update when minute changes

private:
    QLocale m_locale = QLocale::system();
    bool m_hasAlarm = false;
    QString m_string;
    QTimer *m_timer;
    QProcess *m_process;
};
