// SPDX-FileCopyrightText: 2020 Han Young <hanyoung@protonmail.com>
// SPDX-FileCopyrightText: 2020-2024 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "utilmodel.h"
#include "settingsmodel.h"

#include <QLocale>
#include <QQmlEngine>
#include <QStandardPaths>
#include <QTimeZone>
#include <QUrl>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <KSvg/Svg>

UtilModel *UtilModel::instance()
{
    static UtilModel *singleton = new UtilModel;
    return singleton;
}

UtilModel *UtilModel::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine);
    Q_UNUSED(jsEngine);
    auto *model = instance();
    QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);
    return model;
}

UtilModel::UtilModel(QObject *parent)
    : QObject{parent}
{
    connect(SettingsModel::instance(), &SettingsModel::timeFormatChanged, this, &UtilModel::use24HourTimeChanged);
}

QString UtilModel::getDefaultAlarmFileLocation()
{
    return QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("sounds/freedesktop/stereo/alarm-clock-elapsed.oga")))
        .path();
}

QString UtilModel::getCurrentTimeZoneName()
{
    return QTimeZone::systemTimeZone().displayName(QTimeZone::GenericTime);
}

long long UtilModel::calculateNextRingTime(int hours, int minutes, int daysOfWeek, int snooze)
{
    // get the time that the alarm will ring on the day
    QTime alarmTime = QTime(hours, minutes, 0).addSecs(snooze);

    QDateTime date = QDateTime::currentDateTime();

    if (daysOfWeek == 0) { // alarm does not repeat (no days of the week are specified)
        if (alarmTime >= date.time()) { // alarm occurs later today
            return QDateTime(date.date(), alarmTime).toSecsSinceEpoch();
        } else { // alarm occurs on the next day
            return QDateTime(date.date().addDays(1), alarmTime).toSecsSinceEpoch();
        }
    } else { // repeat alarm
        bool first = true;

        // keeping looping forward a single day until the day of week is accepted
        while (((daysOfWeek & (1 << (date.date().dayOfWeek() - 1))) == 0) // check day
               || (first && (alarmTime < date.time()))) // check time if the current day is accepted (keep looping forward if alarmTime has passed)
        {
            date = date.addDays(1); // go forward a day
            first = false;
        }

        return QDateTime(date.date(), alarmTime).toSecsSinceEpoch();
    }
}

QString UtilModel::timeToRingFormatted(const long long &timestamp)
{
    auto remaining = timestamp - QDateTime::currentSecsSinceEpoch();
    int day = remaining / (24 * 3600);
    int hour = remaining / 3600 - day * 24;
    int minute = remaining / 60 - day * 24 * 60 - hour * 60;
    QStringList parts;
    if (day > 0) {
        parts << i18np("%1 day", "%1 days", day);
    }
    if (hour > 0) {
        parts << i18np("%1 hour", "%1 hours", hour);
    }
    if (minute > 0) {
        parts << i18np("%1 minute", "%1 minutes", minute);
    }

    if (day <= 0 && hour <= 0 && minute <= 0) {
        return i18n("Alarm will ring in under a minute");
    } else {
        return i18n("Alarm will ring in %1", QLocale().createSeparatedList(parts));
    }
}

QString UtilModel::timeFormat() const
{
    return use24HourTime() ? QStringLiteral("hh:mm") : QStringLiteral("hh:mm ap");
}

bool UtilModel::use24HourTime() const
{
    QString timeFormat = SettingsModel::instance()->timeFormat();
    if (timeFormat == QStringLiteral("SystemDefault")) {
        return isLocale24HourTime();
    } else {
        return timeFormat == QStringLiteral("24Hour");
    }
}

bool UtilModel::isLocale24HourTime() const
{
    return QLocale::system().timeFormat(QLocale::ShortFormat).toLower().indexOf(QStringLiteral("ap")) == -1;
}

qint64 UtilModel::msToHoursPart(qint64 ms) const
{
    return ms / 1000 / 60 / 60;
}

qint64 UtilModel::msToMinutesPart(qint64 ms) const
{
    return ms / 1000 / 60 - 60 * msToHoursPart(ms);
}

qint64 UtilModel::msToSecondsPart(qint64 ms) const
{
    return ms / 1000 - 60 * msToMinutesPart(ms) - 60 * 60 * msToHoursPart(ms);
}

qint64 UtilModel::msToSmallPart(qint64 ms) const
{
    return ms / 10 - 100 * msToSecondsPart(ms) - 100 * 60 * msToMinutesPart(ms) - 100 * 60 * 60 * msToHoursPart(ms);
}

QString UtilModel::displayTwoDigits(const qint64 &amount)
{
    return QStringLiteral("%1").arg(amount, 2, 10, QLatin1Char('0'));
}

QString UtilModel::repeatFormat(int dayOfWeek) const
{
    if (!dayOfWeek) {
        return i18nc("Repeat", "Only once");
    }
    if (dayOfWeek == (1 << 7) - 1) {
        return i18nc("Repeat", "Everyday");
    }
    if (dayOfWeek == (1 << 5) - 1) {
        return i18nc("Repeat", "Weekdays");
    }

    QLocale locale;

    QList<QString> days;
    days.reserve(7);
    int lastDay = -1;
    for (int day = 0; day < 7; ++day) {
        if (dayOfWeek & (1 << day)) {
            // 0 in QLocale is Sunday.
            const QString dayName = locale.standaloneDayName(day + 1, QLocale::ShortFormat);
            days.append(dayName);
            lastDay = day;
        }
    }

    // Single day, show full name.
    if (days.count() == 1) {
        return locale.standaloneDayName(lastDay + 1, QLocale::LongFormat);
    } else {
        return days.join(i18n(", "));
    }
}

void UtilModel::applyPlasmaImageSet(KSvg::Svg *svg)
{
    if (!m_plasmaImageSet) {
        m_plasmaImageSet = new KSvg::ImageSet(this);
        m_plasmaImageSet->setBasePath(QStringLiteral("plasma/desktoptheme"));
    }

    KSharedConfig::Ptr plasmaRc = KSharedConfig::openConfig(QStringLiteral("plasmarc"));
    const auto themeGroup = plasmaRc->group(QStringLiteral("Theme"));
    m_plasmaImageSet->setImageSetName(themeGroup.readEntry("name", QString()));
    svg->setImageSet(m_plasmaImageSet);
}

#include "moc_utilmodel.cpp"
