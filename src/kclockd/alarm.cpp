/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "alarm.h"

#include "alarmadaptor.h"
#include "kclockdsettings.h"
#include "utilities.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KNotification>
#include <KSharedConfig>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTime>

std::atomic<int> Alarm::ringingCount{0};

void Alarm::bumpRingingCount()
{
    Alarm::ringingCount++;
}

void Alarm::lowerRingingCount()
{
    Alarm::ringingCount--;
}

int Alarm::ringing()
{
    return Alarm::ringingCount;
}

// alarm created from UI
Alarm::Alarm(AlarmModel *parent, QString name, int minutes, int hours, int daysOfWeek)
    : QObject{parent}
    , m_name{name}
    , m_uuid{QUuid::createUuid()}
    , m_hours{hours}
    , m_minutes{minutes}
    , m_daysOfWeek{daysOfWeek}
{
    initialize(parent);
}

// alarm from json (loaded from storage)
Alarm::Alarm(QString serialized, AlarmModel *parent)
    : QObject{parent}
    , m_name{QStringLiteral("New Alarm")}
{
    if (serialized.isEmpty()) {
        m_uuid = QUuid::createUuid();
    } else {
        QJsonDocument doc = QJsonDocument::fromJson(serialized.toUtf8());
        QJsonObject obj = doc.object();

        m_uuid = QUuid::fromString(obj[QStringLiteral("uuid")].toString());
        m_name = obj[QStringLiteral("name")].toString();
        m_minutes = obj[QStringLiteral("minutes")].toInt();
        m_hours = obj[QStringLiteral("hours")].toInt();
        m_daysOfWeek = obj[QStringLiteral("daysOfWeek")].toInt();
        m_enabled = obj[QStringLiteral("enabled")].toBool();
        m_snooze = obj[QStringLiteral("snooze")].toInt();
        m_audioPath = QUrl::fromLocalFile(obj[QStringLiteral("audioPath")].toString());
    }
    initialize(parent);
}

QString Alarm::name() const
{
    return m_name;
}

void Alarm::setName(QString name)
{
    this->m_name = name;
    Q_EMIT nameChanged();
    Q_EMIT propertyChanged(QStringLiteral("name"));
}

QUuid Alarm::uuid() const
{
    return m_uuid;
}

bool Alarm::enabled() const
{
    return m_enabled;
}

void Alarm::setEnabled(bool enabled)
{
    if (this->m_enabled != enabled) {
        this->m_snooze = 0; // reset snooze value
        this->m_nextRingTime = -1; // reset next ring time

        this->m_enabled = enabled;
        Q_EMIT alarmChanged(); // notify the AlarmModel to reschedule
        Q_EMIT enabledChanged();
        Q_EMIT propertyChanged(QStringLiteral("enabled"));
    }
}

int Alarm::hours() const
{
    return m_hours;
}

void Alarm::setHours(int hours)
{
    this->m_hours = hours;
    Q_EMIT alarmChanged();
    Q_EMIT hoursChanged();
    Q_EMIT propertyChanged(QStringLiteral("hours"));
}

int Alarm::minutes() const
{
    return m_minutes;
}

void Alarm::setMinutes(int minutes)
{
    this->m_minutes = minutes;
    Q_EMIT alarmChanged();
    Q_EMIT minutesChanged();
    Q_EMIT propertyChanged(QStringLiteral("minutes"));
}

int Alarm::daysOfWeek() const
{
    return m_daysOfWeek;
}

void Alarm::setDaysOfWeek(int daysOfWeek)
{
    this->m_daysOfWeek = daysOfWeek;
    Q_EMIT alarmChanged();
    Q_EMIT daysOfWeekChanged();
    Q_EMIT propertyChanged(QStringLiteral("daysOfWeek"));
}

int Alarm::snoozedMinutes() const
{
    if (m_snooze != 0 && m_enabled) {
        return m_snooze / 60;
    } else {
        return 0;
    }
}

int Alarm::snooze() const
{
    return m_snooze;
}

void Alarm::setSnooze(int snooze)
{
    this->m_snooze = snooze;
    Q_EMIT snoozedMinutesChanged();
    Q_EMIT propertyChanged(QStringLiteral("snoozedMinutes"));
}

QString Alarm::ringtonePath() const
{
    return m_audioPath.toString();
}

void Alarm::setRingtonePath(QString path)
{
    m_audioPath = QUrl(path);
    Q_EMIT ringtonePathChanged();
    Q_EMIT propertyChanged(QStringLiteral("ringtonePath"));
}

QString Alarm::getUUID()
{
    return m_uuid.toString();
}

void Alarm::initialize(AlarmModel *parent)
{
    connect(this, &Alarm::alarmChanged, this, &Alarm::save);
    connect(this, &Alarm::alarmChanged, this, &Alarm::calculateNextRingTime); // the slots will be called according to the order they have been connected.
                                                                              // always connect this before calling AlarmModel::scheduleAlarm

    calculateNextRingTime();

    if (parent) {
        connect(this, &Alarm::alarmChanged, parent, &AlarmModel::scheduleAlarm); // connect this last
    }

    // DBus
    new AlarmAdaptor(this);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/Alarms/") + this->uuid().toString(QUuid::Id128), this);
    connect(this, &QObject::destroyed, [this] {
        QDBusConnection::sessionBus().unregisterObject(QStringLiteral("/Alarms/") + this->uuid().toString(QUuid::Id128), QDBusConnection::UnregisterNode);
    });
}

// alarm to json
QString Alarm::serialize()
{
    QJsonObject obj;
    obj[QStringLiteral("uuid")] = uuid().toString();
    obj[QStringLiteral("name")] = name();
    obj[QStringLiteral("minutes")] = minutes();
    obj[QStringLiteral("hours")] = hours();
    obj[QStringLiteral("daysOfWeek")] = daysOfWeek();
    obj[QStringLiteral("enabled")] = enabled();
    obj[QStringLiteral("snooze")] = snooze();
    obj[QStringLiteral("audioPath")] = m_audioPath.toLocalFile();
    QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    return QString::fromStdString(data.toStdString());
}

void Alarm::save()
{
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(ALARM_CFG_GROUP);
    group.writeEntry(uuid().toString(), this->serialize());
    group.sync();
}

void Alarm::ring()
{
    // if not enabled, don't ring
    if (!this->enabled())
        return;
    bumpRingingCount();
    qDebug() << "Ringing alarm" << m_name << "and sending notification...";

    KNotification *notif = new KNotification(QStringLiteral("alarm"));
    notif->setActions(QStringList{i18n("Dismiss"), i18n("Snooze")});
    notif->setIconName(QStringLiteral("kclock"));
    notif->setTitle(name() == QString() ? i18n("Alarm") : name());
    notif->setText(QLocale::system().toString(QTime::currentTime(), QLocale::ShortFormat)); // TODO
    notif->setDefaultAction(i18n("View"));
    notif->setFlags(KNotification::NotificationFlag::Persistent);

    connect(notif, &KNotification::defaultActivated, this, &Alarm::handleDismiss);
    connect(notif, &KNotification::action1Activated, this, &Alarm::handleDismiss);
    connect(notif, &KNotification::action2Activated, this, &Alarm::handleSnooze);
    connect(notif, &KNotification::closed, this, &Alarm::handleDismiss);

    notif->sendEvent();

    QDBusMessage wakeupCall = QDBusMessage::createMethodCall(QStringLiteral("org.kde.Solid.PowerManagement"),
                                                             QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                             QStringLiteral("org.kde.Solid.PowerManagement"),
                                                             QStringLiteral("wakeup"));
    QDBusConnection::sessionBus().call(wakeupCall);

    alarmNotifOpen = true;
    alarmNotifOpenTime = QTime::currentTime();

    // play sound (it will loop)
    qDebug() << "Alarm sound: " << m_audioPath;
    AlarmPlayer::instance().setSource(this->m_audioPath);
    AlarmPlayer::instance().play();
}

void Alarm::handleDismiss()
{
    alarmNotifOpen = false;

    qDebug() << "Alarm" << m_name << "dismissed";
    AlarmPlayer::instance().stop();

    // ignore if the snooze button was pressed and dismiss is still called
    if (!m_justSnoozed) {
        // disable alarm if set to run once
        if (daysOfWeek() == 0) {
            setEnabled(false);
        }
    } else {
        qDebug() << "Ignore dismiss (triggered by snooze)" << m_snooze;
    }

    m_justSnoozed = false;

    save();
    Q_EMIT alarmChanged();
    lowerRingingCount();
    Utilities::instance().decfActiveCount();
}

void Alarm::handleSnooze()
{
    m_justSnoozed = true;

    alarmNotifOpen = false;
    qDebug() << "Alarm snoozed (" << KClockSettings::self()->alarmSnoozeLength() << ")";
    AlarmPlayer::instance().stop();

    setSnooze(snooze() + 60 * KClockSettings::self()->alarmSnoozeLength()); // snooze 5 minutes
    m_enabled = true; // can't use setSnooze because it resets snooze time
    save();
    Q_EMIT alarmChanged();
    lowerRingingCount();
    Utilities::instance().decfActiveCount();
}

void Alarm::calculateNextRingTime()
{
    if (!this->m_enabled) { // if not enabled, means this would never ring
        m_nextRingTime = 0;
        return;
    }

    // get the time that the alarm will ring on the day
    QTime alarmTime = QTime(this->m_hours, this->m_minutes, 0).addSecs(this->m_snooze);

    QDateTime date = QDateTime::currentDateTime();

    if (this->m_daysOfWeek == 0) { // alarm does not repeat (no days of the week are specified)
        if (alarmTime >= date.time()) { // alarm occurs later today
            m_nextRingTime = QDateTime(date.date(), alarmTime).toSecsSinceEpoch();
        } else { // alarm occurs on the next day
            m_nextRingTime = QDateTime(date.date().addDays(1), alarmTime).toSecsSinceEpoch();
        }
    } else { // repeat alarm
        bool first = true;

        // keeping looping forward a single day until the day of week is accepted
        while (((this->m_daysOfWeek & (1 << (date.date().dayOfWeek() - 1))) == 0) // check day
               || (first && (alarmTime < date.time()))) // check time if the current day is accepted (keep looping forward if alarmTime has passed)
        {
            date = date.addDays(1); // go forward a day
            first = false;
        }

        m_nextRingTime = QDateTime(date.date(), alarmTime).toSecsSinceEpoch();
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
