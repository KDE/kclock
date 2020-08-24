/*
    SPDX-FileCopyrightText: 2020 HanY <hanyoung@protonmail.com>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kclock_kweather_3x3.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QProcess>
#include <QTimer>
#include <klocalizedstring.h>
#include <QXmlStreamReader>
#include <QJsonDocument>
#include <QJsonArray>
KClock_KWeather_3x3::KClock_KWeather_3x3(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args)
{
    // KWeather
    auto system = QLocale::system().measurementSystem();
    if (system == QLocale::MetricSystem || system == QLocale::ImperialUKSystem)
        m_isCelsius = true;
    else
        m_isCelsius = false;
    QDBusConnection::sessionBus().connect("org.kde.kweather", "/", "org.kde.kweather.LocationModel", "removed", this, SLOT(addCity(QString)));
    QDBusConnection::sessionBus().connect("org.kde.kweather", "/", "org.kde.kweather.LocationModel", "added", this, SLOT(removeCity(QString)));
    QDBusInterface *interface = new QDBusInterface("org.kde.kweather", "/locations", "org.freedesktop.DBus.Introspectable", QDBusConnection::sessionBus(), this);
    QDBusReply<QString> reply = interface->call("Introspect");
    if (reply.isValid()) {
        auto xmlMsg = reply.value();
        QXmlStreamReader xml(xmlMsg);
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.name() == "node" && xml.attributes().hasAttribute("name")){
                m_location = xml.attributes().value("name").toString();
                break;
            }
        }
    }
    delete interface;
    if (!m_location.isEmpty()) {
        m_KWeatherInterface = new QDBusInterface("org.kde.kweather", "/locations/" + m_location, "org.kde.kweather.WeatherLocation", QDBusConnection::sessionBus(), this);
        QDBusConnection::sessionBus().connect("org.kde.kweather", "/locations/" + m_location, "org.kde.kweather.WeatherLocation", "currentForecastChange", this, SLOT(update()));

        QDBusReply<QString> reply = m_KWeatherInterface->call("getWeatherData");
        this->parse(QJsonDocument::fromJson(reply.value().toUtf8()));
        this->m_cityName = m_KWeatherInterface->property("name").toString();
        Q_EMIT locationChanged();
    }

    // KClock
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &KClock_KWeather_3x3::initialTimeUpdate);
    m_timer->setSingleShot(true);

    m_minutesCounter = QTime::currentTime().msecsSinceStartOfDay() / (1000 * 60); // seconds since start of the day
    // initial interval is milliseconds to next minute
    m_timer->start((60 - (QTime::currentTime().msecsSinceStartOfDay() / 1000) % 60) * 1000); // seconds to next minute

    if (!QDBusConnection::sessionBus().connect("org.kde.kclock", "/alarms", "org.kde.kclock.AlarmModel", "nextAlarm", this, SLOT(updateAlarm(qulonglong))))
        m_string = "connect failed";

    interface = new QDBusInterface("org.kde.kclock", "/alarms", "org.kde.kclock.AlarmModel", QDBusConnection::sessionBus(), this);
    QDBusReply<quint64> KClock_reply = interface->call("getNextAlarm");
    if (reply.isValid()) {
        auto alarmTime = KClock_reply.value();
        if (alarmTime > 0) {
            auto dateTime = QDateTime::fromSecsSinceEpoch(alarmTime).toLocalTime();
            m_string = m_local.standaloneDayName(dateTime.date().dayOfWeek(), QLocale::ShortFormat) + " " + m_local.toString(dateTime.time(), QLocale::ShortFormat);
            m_hasAlarm = true;
        } else
            m_hasAlarm = false;
    }
    emit propertyChanged();
}

void KClock_KWeather_3x3::parse(QJsonDocument doc)
{
    QJsonArray dailyArray = doc["dailyForecasts"].toArray();
    QJsonArray hourlyArray = doc["hourlyForecasts"].toArray();
    m_description = hourlyArray.at(0).toObject()["weatherDescription"].toString();
    m_weatherIcon = hourlyArray.at(0).toObject()["weatherIcon"].toString();
    if (m_isCelsius)
        m_tempNow = QString::number(hourlyArray.at(0).toObject()["temperature"].toDouble()) + "°C";
    else
        m_tempNow = QString::number(hourlyArray.at(0).toObject()["temperature"].toDouble() * 1.8 + 32) + "°";

    if (m_isCelsius){
        m_maxTemp = static_cast<int>(dailyArray.first().toObject()["maxTemp"].toDouble());
        m_minTemp = static_cast<int>(dailyArray.first().toObject()["minTemp"].toDouble());
    }
    else{
        m_maxTemp = dailyArray.first().toObject()["maxTemp"].toDouble() * 1.8 + 32;
        m_minTemp = dailyArray.first().toObject()["minTemp"].toDouble() * 1.8 + 32;
    }
    Q_EMIT dataUpdated();
}
void KClock_KWeather_3x3::updateAlarm(qulonglong time)
{
    auto dateTime = QDateTime::fromSecsSinceEpoch(time).toLocalTime();
    if (time > 0) {
        m_string = m_local.standaloneDayName(dateTime.date().dayOfWeek(), QLocale::ShortFormat) + " " + m_local.toString(dateTime.time(), QLocale::ShortFormat);
        m_hasAlarm = true;
    } else {
        m_hasAlarm = false;
    }
    emit propertyChanged();
}
void KClock_KWeather_3x3::openKClock()
{
    m_process = new QProcess(this);
    m_process->start("kclock", QStringList());
}
void KClock_KWeather_3x3::initialTimeUpdate()
{
    emit timeChanged();
    disconnect(m_timer, &QTimer::timeout, this, &KClock_KWeather_3x3::initialTimeUpdate); // disconnect
    m_timer->setSingleShot(false);
    connect(m_timer, &QTimer::timeout, this, &KClock_KWeather_3x3::updateTime);
    m_timer->start(60000); // update every minute
}

void KClock_KWeather_3x3::updateTime(){
    Q_EMIT timeChanged();
    m_minutesCounter++;
    if(m_minutesCounter >= 60 * 24){
        m_minutesCounter = 0;
        Q_EMIT dateChanged();
    }
}
QString KClock_KWeather_3x3::time()
{
    return m_local.toString(QTime::currentTime(), QLocale::ShortFormat);
}
QString KClock_KWeather_3x3::date(){
    return m_local.standaloneDayName(QDate::currentDate().dayOfWeek()) + ", " + m_local.standaloneMonthName(QDate::currentDate().month(), QLocale::ShortFormat) + " " + QString::number(QDate::currentDate().daysInMonth());
}
KClock_KWeather_3x3::~KClock_KWeather_3x3()
{
}

K_EXPORT_PLASMA_APPLET_WITH_JSON(kclock_kweather_3x3, KClock_KWeather_3x3, "metadata.json")

#include "kclock_kweather_3x3.moc"
