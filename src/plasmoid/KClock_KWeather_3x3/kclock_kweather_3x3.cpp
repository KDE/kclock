/*
    SPDX-FileCopyrightText: 2020 HanY <hanyoung@protonmail.com>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kclock_kweather_3x3.h"

#include <KLocalizedString>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
#include <QTimer>
#include <QXmlStreamReader>

KClock_KWeather_3x3::KClock_KWeather_3x3(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args)
{
    // KWeather
    auto system = QLocale::system().measurementSystem();
    if (system == QLocale::MetricSystem || system == QLocale::ImperialUKSystem)
        m_isCelsius = true;
    else
        m_isCelsius = false;
    QDBusConnection::sessionBus().connect(QStringLiteral("org.kde.kweather"),
                                          QStringLiteral("/"),
                                          QStringLiteral("org.kde.kweather.LocationModel"),
                                          QStringLiteral("removed"),
                                          this,
                                          SLOT(addCity(QString)));
    QDBusConnection::sessionBus().connect(QStringLiteral("org.kde.kweather"),
                                          QStringLiteral("/"),
                                          QStringLiteral("org.kde.kweather.LocationModel"),
                                          QStringLiteral("added"),
                                          this,
                                          SLOT(removeCity(QString)));
    QDBusInterface *interface = new QDBusInterface(QStringLiteral("org.kde.kweather"),
                                                   QStringLiteral("/locations"),
                                                   QStringLiteral("org.freedesktop.DBus.Introspectable"),
                                                   QDBusConnection::sessionBus(),
                                                   this);
    QDBusReply<QString> reply = interface->call(QStringLiteral("Introspect"));
    if (reply.isValid()) {
        auto xmlMsg = reply.value();
        QXmlStreamReader xml(xmlMsg);
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.name() == "node" && xml.attributes().hasAttribute("name")) {
                m_location = xml.attributes().value("name").toString();
                break;
            }
        }
    }
    delete interface;
    if (!m_location.isEmpty()) {
        m_KWeatherInterface = new QDBusInterface(QStringLiteral("org.kde.kweather"),
                                                 QStringLiteral("/locations/") + m_location,
                                                 QStringLiteral("org.kde.kweather.WeatherLocation"),
                                                 QDBusConnection::sessionBus(),
                                                 this);
        QDBusConnection::sessionBus().connect(QStringLiteral("org.kde.kweather"),
                                              QStringLiteral("/locations/") + m_location,
                                              QStringLiteral("org.kde.kweather.WeatherLocation"),
                                              QStringLiteral("currentForecastChange"),
                                              this,
                                              SLOT(update()));

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

    if (!QDBusConnection::sessionBus().connect(QStringLiteral("org.kde.kclockd"),
                                               QStringLiteral("/Alarms"),
                                               QStringLiteral("org.kde.kclock.AlarmModel"),
                                               QStringLiteral("nextAlarm"),
                                               this,
                                               SLOT(updateAlarm(qulonglong))))
        m_string = QStringLiteral("connect failed");

    interface = new QDBusInterface(QStringLiteral("org.kde.kclockd"),
                                   QStringLiteral("/Alarms"),
                                   QStringLiteral("org.kde.kclock.AlarmModel"),
                                   QDBusConnection::sessionBus(),
                                   this);
    QDBusReply<quint64> KClock_reply = interface->call(QStringLiteral("getNextAlarm"));
    if (reply.isValid()) {
        auto alarmTime = KClock_reply.value();
        if (alarmTime > 0) {
            auto dateTime = QDateTime::fromSecsSinceEpoch(alarmTime).toLocalTime();
            m_string =
                m_local.standaloneDayName(dateTime.date().dayOfWeek(), QLocale::ShortFormat) + " " + m_local.toString(dateTime.time(), QStringLiteral("hh:mm"));
            m_hasAlarm = true;
        } else
            m_hasAlarm = false;
    }
    emit propertyChanged();
}

void KClock_KWeather_3x3::parse(QJsonDocument doc)
{
    QJsonArray dailyArray = doc[QStringLiteral("dailyForecasts")].toArray();
    QJsonArray hourlyArray = doc[QStringLiteral("hourlyForecasts")].toArray();
    m_description = hourlyArray.at(0).toObject()[QStringLiteral("weatherDescription")].toString();
    m_weatherIcon = hourlyArray.at(0).toObject()[QStringLiteral("weatherIcon")].toString();
    if (m_isCelsius)
        m_tempNow = QString::number(hourlyArray.at(0).toObject()[QStringLiteral("temperature")].toDouble()) + QStringLiteral("°C");
    else
        m_tempNow = QString::number(hourlyArray.at(0).toObject()[QStringLiteral("temperature")].toDouble() * 1.8 + 32) + QStringLiteral("°");

    if (m_isCelsius) {
        m_maxTemp = static_cast<int>(dailyArray.first().toObject()[QStringLiteral("maxTemp")].toDouble());
        m_minTemp = static_cast<int>(dailyArray.first().toObject()[QStringLiteral("minTemp")].toDouble());
    } else {
        m_maxTemp = dailyArray.first().toObject()[QStringLiteral("maxTemp")].toDouble() * 1.8 + 32;
        m_minTemp = dailyArray.first().toObject()[QStringLiteral("minTemp")].toDouble() * 1.8 + 32;
    }
    Q_EMIT dataUpdated();
}
void KClock_KWeather_3x3::updateAlarm(qulonglong time)
{
    auto dateTime = QDateTime::fromSecsSinceEpoch(time).toLocalTime();
    if (time > 0) {
        m_string = m_local.standaloneDayName(dateTime.date().dayOfWeek(), QLocale::ShortFormat) + QStringLiteral(" ")
            + m_local.toString(dateTime.time(), QStringLiteral("hh:mm"));
        m_hasAlarm = true;
    } else {
        m_hasAlarm = false;
    }
    emit propertyChanged();
}
void KClock_KWeather_3x3::openKClock()
{
    m_process = new QProcess(this);
    m_process->start(QStringLiteral("kclock"), QStringList());
}
void KClock_KWeather_3x3::initialTimeUpdate()
{
    emit timeChanged();
    disconnect(m_timer, &QTimer::timeout, this, &KClock_KWeather_3x3::initialTimeUpdate); // disconnect
    m_timer->setSingleShot(false);
    connect(m_timer, &QTimer::timeout, this, &KClock_KWeather_3x3::updateTime);
    m_timer->start(60000); // update every minute
}

void KClock_KWeather_3x3::updateTime()
{
    Q_EMIT timeChanged();
    m_minutesCounter++;
    if (m_minutesCounter >= 60 * 24) {
        m_minutesCounter = 0;
        Q_EMIT dateChanged();
    }
}
QString KClock_KWeather_3x3::time()
{
    return m_local.toString(QTime::currentTime(), QStringLiteral("hh:mm"));
}
QString KClock_KWeather_3x3::date()
{
    return m_local.toString(QDate::currentDate(), QStringLiteral("ddd, MMMM d"));
}
KClock_KWeather_3x3::~KClock_KWeather_3x3()
{
}

K_EXPORT_PLASMA_APPLET_WITH_JSON(kclock_kweather_3x3, KClock_KWeather_3x3, "metadata.json")

#include "kclock_kweather_3x3.moc"
