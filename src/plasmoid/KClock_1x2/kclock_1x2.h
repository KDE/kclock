/*
    SPDX-FileCopyrightText: 2020 HanY <hanyoung@protonmail.com>
    SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/
#pragma once
#include <Plasma/Applet>
#include <QLocale>

class QProcess;
class QTimer;
class KClock_1x2 : public Plasma::Applet
{
    Q_OBJECT
    Q_PROPERTY(QString time READ time NOTIFY timeChanged)
    Q_PROPERTY(QString date READ date NOTIFY timeChanged)
    Q_PROPERTY(QString alarmTime READ alarmTime NOTIFY propertyChanged)
    Q_PROPERTY(bool hasAlarm READ hasAlarm NOTIFY propertyChanged)

public:
    KClock_1x2(QObject *parent, const QVariantList &args);
    ~KClock_1x2();

    QString time();
    QString date();
    QString alarmTime()
    {
        return m_string;
    };
    bool hasAlarm()
    {
        return m_hasAlarm;
    }
    Q_INVOKABLE void openKClock();

signals:
    void propertyChanged();
    void timeChanged();

private slots:
    void updateAlarm(qulonglong time);
    void initialTimeUpdate(); // making sure time is update when minute changes

private:
    QLocale m_locale = QLocale::system();
    bool m_hasAlarm = false;
    QString m_string;
    QTimer *m_timer;
    QProcess *m_process;
};
