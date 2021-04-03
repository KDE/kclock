/*
    SPDX-FileCopyrightText: 2020 HanY <hanyoung@protonmail.com>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#pragma once
#include <Plasma/Applet>
#include <QLocale>

class QProcess;
class QTimer;
class QDBusInterface;
class KClock_KWeather_3x3 : public Plasma::Applet
{
    Q_OBJECT
    Q_PROPERTY(QString time READ time NOTIFY timeChanged)
    Q_PROPERTY(QString date READ date NOTIFY dateChanged)
    Q_PROPERTY(QString alarmTime READ alarmTime NOTIFY propertyChanged)
    Q_PROPERTY(bool hasAlarm READ hasAlarm NOTIFY propertyChanged)
    Q_PROPERTY(QString cityName READ cityName NOTIFY locationChanged)
    Q_PROPERTY(QString description READ description NOTIFY dataUpdated)
    Q_PROPERTY(QString weatherIcon READ weatherIcon NOTIFY dataUpdated)
    Q_PROPERTY(QString tempNow READ tempNow NOTIFY dataUpdated)
    Q_PROPERTY(QString maxTemp READ maxTemp NOTIFY dataUpdated)
    Q_PROPERTY(QString minTemp READ minTemp NOTIFY dataUpdated)
public:
    KClock_KWeather_3x3(QObject *parent, const QVariantList &args);
    ~KClock_KWeather_3x3();
    QString time();
    QString date();
    QString alarmTime()
    {
        return m_string;
    };
    bool hasAlarm()
    {
        return m_hasAlarm;
    };
    QString cityName()
    {
        return m_cityName;
    };
    QString description()
    {
        return m_description;
    };
    QString weatherIcon()
    {
        return m_weatherIcon;
    }
    QString tempNow()
    {
        return m_tempNow;
    };
    QString maxTemp()
    {
        QString arg = QString::number(m_maxTemp);
        if (this->m_isCelsius)
            arg += "°C";
        else
            arg += "°";
        return arg;
    };
    QString minTemp()
    {
        QString arg = QString::number(m_minTemp);
        if (this->m_isCelsius)
            arg += "°C";
        else
            arg += "°";
        return arg;
    };
    Q_INVOKABLE void openKClock();
signals:
    void propertyChanged();
    void timeChanged();
    void dataUpdated();
    void locationChanged();
    void dateChanged();
private slots:
    void updateAlarm(qulonglong time);
    void initialTimeUpdate(); // align time to minute mark
    void updateTime();

private:
    void parse(QJsonDocument);

    QDBusInterface *m_KWeatherInterface, *m_KClockInterface;

    QString m_cityName;
    QString m_weatherIcon;
    QString m_description;
    QString m_tempNow;
    QString m_location;
    int m_maxTemp, m_minTemp;
    bool m_isCelsius;

    int m_minutesCounter; // count how many minutes have passed to track day change

    QLocale m_local = QLocale::system();
    bool m_hasAlarm = false;
    QString m_string;
    QTimer *m_timer;
    QProcess *m_process;
};
