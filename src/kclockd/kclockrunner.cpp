/*
 * SPDX-FileCopyrightText: 2025 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kclockrunner.h"

#include "krunner1adaptor.h"

#include "debug_runner.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDir>

#include <KLocalizedString>

#include "alarm.h"
#include "alarmmodel.h"
#include "timer.h"
#include "timermodel.h"

#include <chrono>

using namespace Qt::Literals::StringLiterals;
using namespace std::literals::chrono_literals;

static QString formatDuration(const std::chrono::seconds &duration)
{
    const auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration - hours);
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration - hours - minutes);

    return hours.count() > 0 ? u"%1:%2:%3"_s.arg(hours.count()).arg(minutes.count(), 2, 10, '0'_L1).arg(seconds.count(), 2, 10, '0'_L1)
                             : u"%1:%2"_s.arg(minutes.count()).arg(seconds.count(), 2, 10, '0'_L1);
}

static constexpr QLatin1String timerPrefix()
{
    return "timer_"_L1;
}

static constexpr QLatin1String alarmPrefix()
{
    return "alarm_"_L1;
}

static void openKAlarmPage(const QString &page)
{
    // Bring KClock to the front, if running, basically what KDBusService would be doing.
    QDBusMessage msg = QDBusMessage::createMethodCall(u"org.kde.kclock"_s, u"/org/kde/kclock"_s, u"org.kde.KDBusService"_s, u"CommandLine"_s);
    msg.setArguments({
        QStringList{u"kclock"_s, u"--page"_s, page},
        QDir::currentPath(),
        QVariantMap{}, // TODO Figure out XDG Activation for DBus runner.
    });
    QDBusConnection::sessionBus().call(msg, QDBus::NoBlock);
}

KClockRunner::KClockRunner(QObject *parent)
    : QObject(parent)
{
    new Krunner1Adaptor(this);
    qDBusRegisterMetaType<RemoteMatch>();
    qDBusRegisterMetaType<RemoteMatches>();
    qDBusRegisterMetaType<RemoteAction>();
    qDBusRegisterMetaType<RemoteActions>();
    QDBusConnection::sessionBus().registerObject(u"/runner"_s, this);
}

void KClockRunner::Teardown()
{
}

RemoteActions KClockRunner::Actions() const
{
    return RemoteActions{RemoteAction{u"plus1minute"_s, i18nc("@action:button Add 1 minute to timer", "+1 Minute"), u"list-add"_s},
                         RemoteAction{u"reset"_s, i18nc("@action:button", "Reset Timer"), u"chronometer-reset"_s}};
}

RemoteMatches KClockRunner::Match(const QString &searchTerm)
{
    RemoteMatches matches;

    const QString timerKeyword = i18nc("Note this is a KRunner keyword", "timer");
    const QString timerQuery = searchTerm.startsWith(timerKeyword, Qt::CaseInsensitive) ? searchTerm.mid(timerKeyword.length() + 1) : searchTerm;

    const auto timers = TimerModel::instance()->timerList();
    for (Timer *timer : timers) {
        if (!timerQuery.isEmpty() && !timer->label().contains(timerQuery, Qt::CaseInsensitive)) {
            continue;
        }

        RemoteMatch match;
        match.id = timerPrefix() + timer->uuid();
        match.categoryRelevance = HighestCategoryRelevance;
        match.iconName = timer->running() ? u"chronometer-pause"_s : u"chronometer-start"_s;
        const auto length = std::chrono::seconds{timer->length()};
        const auto elapsed = std::chrono::seconds{timer->elapsed()};
        const auto remaining = length - elapsed;
        if (timer->running() || timer->elapsed()) {
            match.text = i18nc("Timer running or paused (remaining time)", "%1 (%2 Remaining)", timer->label(), formatDuration(remaining));
        } else {
            match.text = i18nc("Timer not running (timer length)", "%1 (%2)", timer->label(), formatDuration(length));
        }

        match.relevance = 0.5;
        if (timer->label().startsWith(searchTerm, Qt::CaseInsensitive)) {
            match.relevance += 0.2;
            if (timer->label().compare(searchTerm, Qt::CaseInsensitive) == 0) {
                match.relevance = 1.0;
            }
        }

        match.properties.insert(u"category"_s, i18nc("KRunner category", "Timer"));

        QString subtext;
        if (timer->running()) {
            subtext = i18nc("@action", "Pause");
        } else if (timer->elapsed()) {
            subtext = i18nc("@action", "Resume");
        } else {
            subtext = i18nc("@action", "Start Timer");
        }
        match.properties.insert(u"subtext"_s, subtext);

        QStringList actions;
        actions << u"plus1minute"_s;
        if (timer->elapsed()) {
            actions << u"reset"_s;
        }

        match.properties.insert(u"actions"_s, actions);

        matches << match;
    }

    const QString alarmKeyword = i18nc("Note this is a KRunner keyword", "alarm");
    const QString alarmQuery = searchTerm.startsWith(alarmKeyword, Qt::CaseInsensitive) ? searchTerm.mid(alarmKeyword.length() + 1) : searchTerm;

    const auto alarms = AlarmModel::instance()->alarmsList();
    for (Alarm *alarm : alarms) {
        if (!alarmQuery.isEmpty() && !alarm->name().contains(alarmQuery, Qt::CaseInsensitive)) {
            continue;
        }

        RemoteMatch match;
        match.id = alarmPrefix() + alarm->uuid();
        match.categoryRelevance = HighestCategoryRelevance;
        if (alarm->ringing()) {
            match.iconName = u"notification-active"_s;
        } else if (alarm->enabled()) {
            match.iconName = u"notification-inactive"_s;
        } else {
            match.iconName = u"notification-disabled"_s;
        }

        const QString timeString = u"%1:%2"_s.arg(alarm->hours(), 2, 10, '0'_L1).arg(alarm->minutes(), 2, 10, '0'_L1);
        // TODO weekdays.
        match.text = !alarm->name().isEmpty() ? i18nc("Alarm name (time)", "%1 (%2)", alarm->name(), timeString) : timeString;

        match.relevance = 0.5;
        if (alarm->name().startsWith(searchTerm, Qt::CaseInsensitive)) {
            match.relevance += 0.2;
            if (alarm->name().compare(searchTerm, Qt::CaseInsensitive) == 0) {
                match.relevance = 1.0;
            }
        }

        match.properties.insert(u"category"_s, i18nc("KRunner category", "Alarm"));

        QString subtext;
        if (alarm->ringing()) {
            subtext = i18nc("@action Dismiss Alarm", "Dismiss");
        } else if (alarm->enabled()) {
            const auto nextRingTimeDelta = std::chrono::seconds{alarm->nextRingTime()} - std::chrono::seconds{QDateTime::currentSecsSinceEpoch()};
            subtext = i18nc("Disable timer (rings in time)", "Disable (Rings in %1)", formatDuration(nextRingTimeDelta));
        } else {
            subtext = i18nc("@action Enable Alarm", "Enable");
        }

        match.properties.insert(u"subtext"_s, subtext);

        match.properties.insert(u"actions"_s, QStringList{});

        matches << match;
    }

    return matches;
}

void KClockRunner::Run(const QString &id, const QString &actionId)
{
    if (id.startsWith(timerPrefix())) {
        const QString timerId = id.mid(timerPrefix().size());
        Timer *timer = TimerModel::instance()->timer(timerId);
        if (!timer) {
            qCWarning(RUNNER_DEBUG) << "Failed to find timer with id" << timerId;
            return;
        }

        if (actionId == "plus1minute"_L1) {
            qCDebug(RUNNER_DEBUG) << "Add 1 minute" << timer->label();
            const auto newLength = timer->length() + 60;
            timer->setLength(newLength);
        } else if (actionId == "reset"_L1) {
            qCDebug(RUNNER_DEBUG) << "Reset timer" << timer->label();
            timer->reset();
        } else if (actionId.isEmpty()) {
            qCDebug(RUNNER_DEBUG) << "Toggle running" << timer->label();

            if (!timer->running()) {
                openKAlarmPage(u"Timers"_s);
            }

            // TODO show a KNotification that confirms the timer having been (re)started.
            timer->toggleRunning();
        } else {
            qCWarning(RUNNER_DEBUG) << "Invalid action id to run" << actionId;
        }

    } else if (id.startsWith(alarmPrefix())) {
        const QString alarmId = id.mid(alarmPrefix().size());
        Alarm *alarm = AlarmModel::instance()->alarm(alarmId);
        if (!alarm) {
            qCWarning(RUNNER_DEBUG) << "Failed to find alarm with id" << alarmId;
            return;
        }

        if (actionId.isEmpty()) {
            if (alarm->ringing()) {
                qCDebug(RUNNER_DEBUG) << "Dismissed alarm" << alarm->name();
                alarm->dismiss();
            } else if (alarm->enabled()) {
                qCDebug(RUNNER_DEBUG) << "Disabled alarm" << alarm->name();
                alarm->setEnabled(false);
            } else {
                qCDebug(RUNNER_DEBUG) << "Enabled alarm" << alarm->name();
                openKAlarmPage(u"Alarms"_s);
                // TODO show a KNotification that confirms the alarm having been enabled/disabled.
                alarm->setEnabled(true);
            }
        } else {
            qCWarning(RUNNER_DEBUG) << "Invalid action id to run" << actionId;
        }

    } else {
        qCWarning(RUNNER_DEBUG) << "Invalid id to run" << id;
    }
}

#include "moc_kclockrunner.cpp"
