/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2022 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "alarm.h"

#include "alarmadaptor.h"
#include "kclockdsettings.h"
#include "utilities.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

// alarm from json (loaded from storage)
Alarm::Alarm(QString serialized, AlarmModel *parent)
    : QObject{parent}
{
    if (!serialized.isEmpty()) {
        QJsonDocument doc = QJsonDocument::fromJson(serialized.toUtf8());
        QJsonObject obj = doc.object();

        m_uuid = QUuid::fromString(obj[QStringLiteral("uuid")].toString());
        m_name = obj[QStringLiteral("name")].toString();
        m_enabled = obj[QStringLiteral("enabled")].toBool();
        m_hours = obj[QStringLiteral("hours")].toInt();
        m_minutes = obj[QStringLiteral("minutes")].toInt();
        m_daysOfWeek = obj[QStringLiteral("daysOfWeek")].toInt();
        m_audioPath = QUrl::fromLocalFile(obj[QStringLiteral("audioPath")].toString());
        m_ringDuration = obj[QStringLiteral("ringDuration")].toInt(5);
        m_snoozeDuration = obj[QStringLiteral("snoozeDuration")].toInt(5);
        m_snoozedLength = obj[QStringLiteral("snoozedLength")].toInt();
    }

    init(parent);
}

Alarm::Alarm(QString name, int hours, int minutes, int daysOfWeek, QString audioPath, int ringDuration, int snoozeDuration, AlarmModel *parent)
    : QObject{parent}
    , m_uuid{QUuid::createUuid()}
    , m_name{name}
    , m_enabled{true} // default the alarm to be on
    , m_hours{hours}
    , m_minutes{minutes}
    , m_daysOfWeek{daysOfWeek}
    , m_audioPath{QUrl::fromLocalFile(audioPath.replace(QStringLiteral("file://"), QString()))}
    , m_ringDuration{ringDuration}
    , m_snoozeDuration{snoozeDuration}
{
    init(parent);
}

Alarm::~Alarm()
{
    // ensure alarm doesn't continue ringing after deletion
    if (m_ringing) {
        AlarmPlayer::instance().stop();
    }
}

void Alarm::init(AlarmModel *parent)
{
    // setup notification
    m_notification->setActions({i18n("Dismiss"), i18n("Snooze")});
    m_notification->setIconName(QStringLiteral("kclock"));
    m_notification->setTitle(name() == QString() ? i18n("Alarm") : name());
    m_notification->setText(QLocale::system().toString(QTime::currentTime(), QLocale::ShortFormat));
    m_notification->setDefaultAction(i18n("View"));
    m_notification->setAutoDelete(false); // don't auto-delete when closing

    connect(m_notification, &KNotification::defaultActivated, this, &Alarm::dismiss);
    connect(m_notification, &KNotification::action1Activated, this, &Alarm::dismiss);
    connect(m_notification, &KNotification::action2Activated, this, &Alarm::snooze);
    connect(m_notification, &KNotification::closed, this, &Alarm::dismiss);

    // setup signals
    connect(this, &Alarm::rescheduleRequested, this, &Alarm::save);
    connect(this, &Alarm::rescheduleRequested, this, &Alarm::calculateNextRingTime);
    calculateNextRingTime();

    if (parent) {
        // must be after calculateNextTime signal, to ensure the ring time is calculated before the alarm is scheduled
        connect(this, &Alarm::rescheduleRequested, parent, &AlarmModel::scheduleAlarm);
    }

    // setup dbus
    new AlarmAdaptor(this);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/Alarms/") + m_uuid.toString(QUuid::Id128), this);
    connect(this, &QObject::destroyed, [this] {
        QDBusConnection::sessionBus().unregisterObject(QStringLiteral("/Alarms/") + m_uuid.toString(QUuid::Id128), QDBusConnection::UnregisterNode);
    });

    // setup ringing length timer
    connect(m_ringTimer, &QTimer::timeout, this, []() {
        AlarmPlayer::instance().stop();
    });
}

// alarm to json
QString Alarm::serialize()
{
    QJsonObject obj;
    obj[QStringLiteral("uuid")] = m_uuid.toString();
    obj[QStringLiteral("name")] = m_name;
    obj[QStringLiteral("enabled")] = m_enabled;
    obj[QStringLiteral("hours")] = m_hours;
    obj[QStringLiteral("minutes")] = m_minutes;
    obj[QStringLiteral("daysOfWeek")] = m_daysOfWeek;
    obj[QStringLiteral("audioPath")] = m_audioPath.toLocalFile();
    obj[QStringLiteral("ringDuration")] = m_ringDuration;
    obj[QStringLiteral("snoozeDuration")] = m_snoozeDuration;
    obj[QStringLiteral("snoozedLength")] = m_snoozedLength;
    QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    return QString::fromStdString(data.toStdString());
}

QString Alarm::uuid() const
{
    return m_uuid.toString();
}

QString Alarm::name() const
{
    return m_name;
}

void Alarm::setName(QString name)
{
    if (name != m_name) {
        m_name = name;
        Q_EMIT nameChanged();
    }
}

bool Alarm::enabled() const
{
    return m_enabled;
}

void Alarm::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        setSnoozedLength(0); // reset snooze value
        setNextRingTime(0); // reset next ring time
        m_enabled = enabled;
        Q_EMIT enabledChanged();
        Q_EMIT rescheduleRequested(); // notify the AlarmModel to reschedule
    }
}

int Alarm::hours() const
{
    return m_hours;
}

void Alarm::setHours(int hours)
{
    if (hours != m_hours) {
        m_hours = hours;
        Q_EMIT hoursChanged();
        Q_EMIT rescheduleRequested();
    }
}

int Alarm::minutes() const
{
    return m_minutes;
}

void Alarm::setMinutes(int minutes)
{
    if (minutes != m_minutes) {
        m_minutes = minutes;
        Q_EMIT minutesChanged();
        Q_EMIT rescheduleRequested();
    }
}

int Alarm::daysOfWeek() const
{
    return m_daysOfWeek;
}

void Alarm::setDaysOfWeek(int daysOfWeek)
{
    if (daysOfWeek != m_daysOfWeek) {
        m_daysOfWeek = daysOfWeek;
        Q_EMIT daysOfWeekChanged();
        Q_EMIT rescheduleRequested();
    }
}

QString Alarm::audioPath() const
{
    return m_audioPath.toLocalFile();
}

void Alarm::setAudioPath(QString path)
{
    if (m_audioPath.path() != path) {
        path = path.replace(QStringLiteral("file://"), QString());
        m_audioPath = QUrl::fromLocalFile(path);
        Q_EMIT audioPathChanged();
    }
}

int Alarm::ringDuration() const
{
    return m_ringDuration;
}

void Alarm::setRingDuration(int ringDuration)
{
    if (ringDuration != m_ringDuration) {
        m_ringDuration = ringDuration;
        Q_EMIT ringDurationChanged();
    }
}

int Alarm::snoozeDuration() const
{
    return m_snoozeDuration;
}

void Alarm::setSnoozeDuration(int snoozeDuration)
{
    if (snoozeDuration != m_snoozeDuration) {
        m_snoozeDuration = snoozeDuration;
        Q_EMIT snoozeDurationChanged();
    }
}

int Alarm::snoozedLength() const
{
    return m_snoozedLength;
}

void Alarm::setSnoozedLength(int snoozedLength)
{
    if (snoozedLength != m_snoozedLength) {
        m_snoozedLength = snoozedLength;
        Q_EMIT snoozedLengthChanged();
    }
}

bool Alarm::ringing() const
{
    return m_ringing;
}

void Alarm::setRinging(bool ringing)
{
    if (ringing != m_ringing) {
        m_ringing = ringing;
        Q_EMIT ringingChanged();
    }
}

void Alarm::save()
{
    // save this alarm to the config
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    group.writeEntry(m_uuid.toString(), this->serialize());
    group.sync();
}

void Alarm::ring()
{
    if (!m_enabled) {
        return;
    }

    qDebug() << "Ringing alarm" << m_name << "and sending notification...";

    // save ring time
    if (m_snoozedLength == 0) {
        m_originalRingTime = m_nextRingTime;
    }

    // reset snoozed length (if snooze happens, this will get set again)
    setSnoozedLength(0);

    // send notification
    m_notification->setText(QLocale::system().toString(QTime::currentTime(), QLocale::ShortFormat));
    m_notification->sendEvent();

    // wake up device
    Utilities::wakeupNow();

    // play sound (it will loop)
    qDebug() << "Alarm sound: " << m_audioPath;
    AlarmPlayer::instance().setSource(m_audioPath);
    AlarmPlayer::instance().play();
    setRinging(true);

    // stop ringing after duration
    m_ringTimer->start(m_ringDuration * 60 * 1000);
}

void Alarm::dismiss()
{
    qDebug() << "Alarm" << m_name << "dismissed";

    // ignore if the snooze button was pressed and dismiss is still called
    if (!m_justSnoozed && daysOfWeek() == 0) {
        // disable alarm if set to run once
        setEnabled(false);
    }
    m_justSnoozed = false;

    AlarmPlayer::instance().stop();
    setRinging(false);
    m_ringTimer->stop();
    m_notification->close();
    Q_EMIT rescheduleRequested();

    Utilities::instance().decfActiveCount();
}

void Alarm::snooze()
{
    qDebug() << "Alarm snoozed (" << m_snoozeDuration << " minutes)";
    AlarmPlayer::instance().stop();
    setRinging(false);
    m_ringTimer->stop();
    m_notification->close();
    m_justSnoozed = true;

    // add snooze amount, and enable alarm
    setSnoozedLength((QDateTime::currentSecsSinceEpoch() + 60 * m_snoozeDuration) - m_originalRingTime);
    m_enabled = true; // can't use setEnabled, since it will reset snoozedLength
    Q_EMIT enabledChanged();
    Q_EMIT rescheduleRequested();

    Utilities::instance().decfActiveCount();
}

void Alarm::calculateNextRingTime()
{
    if (!this->m_enabled) { // if not enabled, means this would never ring
        setNextRingTime(0);
        return;
    }

    // get the time that the alarm will ring on the day
    QTime alarmTime = QTime(m_hours, m_minutes, 0).addSecs(m_snoozedLength);
    QDateTime date = QDateTime::currentDateTime();

    if (this->m_daysOfWeek == 0) { // alarm does not repeat (no days of the week are specified)
        // either the alarm occurs today or tomorrow
        bool alarmOccursToday = alarmTime >= date.time();
        setNextRingTime(QDateTime(date.date().addDays(alarmOccursToday ? 0 : 1), alarmTime).toSecsSinceEpoch());

    } else { // repeat alarm
        bool first = true;

        // keeping looping forward a single day until the day of week is accepted
        while (((this->m_daysOfWeek & (1 << (date.date().dayOfWeek() - 1))) == 0) // check day
               || (first && (alarmTime < date.time()))) // check time if the current day is accepted (keep looping forward if alarmTime has passed)
        {
            date = date.addDays(1); // go forward a day
            first = false;
        }

        setNextRingTime(QDateTime(date.date(), alarmTime).toSecsSinceEpoch());
    }
}

void Alarm::setNextRingTime(quint64 nextRingTime)
{
    if (m_nextRingTime != nextRingTime) {
        m_nextRingTime = nextRingTime;
        Q_EMIT nextRingTimeChanged();
    }
}

quint64 Alarm::nextRingTime()
{
    // day changed, re-calculate
    if (this->m_nextRingTime <= static_cast<unsigned long long>(QDateTime::currentSecsSinceEpoch())) {
        calculateNextRingTime();
    }
    return m_nextRingTime;
}
