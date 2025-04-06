/*
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "timermodel.h"

#include "debug_timermodel.h"

#include "kclockdsettings.h"
#include "timer.h"
#include "unitylauncher.h"
#include "utilities.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KNotification>
#include <KSharedConfig>

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QProcessEnvironment>

#include <QtNumeric>

#include <chrono>

using namespace std::literals::chrono_literals;
using namespace Qt::Literals::StringLiterals;

const QString TIMERS_CFG_GROUP = QStringLiteral("Timers"), TIMERS_CFG_KEY = QStringLiteral("timersList");

TimerModel *TimerModel::instance()
{
    static TimerModel *singleton = new TimerModel();
    return singleton;
}

TimerModel::TimerModel()
    : QObject(nullptr)
    , m_kclockWatcher(u"org.kde.kclock"_s,
                      QDBusConnection::sessionBus(),
                      QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration)
    , m_unityLauncher(new UnityLauncher(this))
{
    // Match what app uses, reduces a possible 1s gap between task bar and app
    // when one or the other ever so slightly misses the second changing.
    m_updateIndicatorsTimer.setInterval(250ms);
    m_updateIndicatorsTimer.callOnTimeout(this, &TimerModel::updateIndicators);

    connect(&m_kclockWatcher, &QDBusServiceWatcher::serviceRegistered, this, &TimerModel::onServiceRegistered);
    connect(&m_kclockWatcher, &QDBusServiceWatcher::serviceUnregistered, this, &TimerModel::onServiceUnregistered);
    m_kclockRunning = QDBusConnection::sessionBus().interface()->isServiceRegistered(m_kclockWatcher.watchedServices().constFirst());

    connect(KClockSettings::self(), &KClockSettings::timerNotificationChanged, this, &TimerModel::onTimerNotificationSettingChanged);

    load();
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/Timers"), this, QDBusConnection::ExportScriptableContents);
}

void TimerModel::load()
{
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(TIMERS_CFG_GROUP);
    QJsonDocument doc = QJsonDocument::fromJson(group.readEntry(TIMERS_CFG_KEY, "{}").toUtf8());
    for (QJsonValueRef r : doc.array()) {
        QJsonObject obj = r.toObject();
        Timer *timer = new Timer(obj, this);
        Q_ASSERT(!timer->running());
        connectTimer(timer);
        m_timerList.append(timer);
    }
    updateIndicators();
}

void TimerModel::save()
{
    QJsonArray arr;
    for (auto timer : m_timerList) {
        arr.push_back(timer->serialize());
    }
    QJsonObject obj;
    obj[QStringLiteral("list")] = arr;

    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(TIMERS_CFG_GROUP);
    QByteArray data = QJsonDocument(arr).toJson(QJsonDocument::Compact);
    group.writeEntry(TIMERS_CFG_KEY, QString::fromStdString(data.toStdString()));

    group.sync();
}

void TimerModel::addTimer(int length, const QString &label, const QString &commandTimeout, bool running)
{
    auto *timer = new Timer(length, label, commandTimeout, running);
    connectTimer(timer);
    m_timerList.append(timer);
    maybeCreateNotification(timer);

    save();
    updateIndicators();

    Q_EMIT timerAdded(timer->uuid());
}

void TimerModel::connectTimer(Timer *timer)
{
    connect(timer, &Timer::runningChanged, this, [this, timer] {
        if (timer->running()) {
            maybeCreateNotification(timer);
            // if no elapsed, it was reset, if elapsed equals length timer is finished.
        } else if (!timer->elapsed() || timer->elapsed() == timer->length()) {
            if (auto *notification = m_notifications.value(timer)) {
                // Avoid closed signal to remove it when a new one has just been sent again (looping timer).
                disconnect(notification, &KNotification::closed, this, nullptr);
                notification->close();
                m_notifications.remove(timer);
            }
        }
        updateIndicators();
    });
    connect(timer, &Timer::lengthChanged, this, &TimerModel::updateIndicators);
}

void TimerModel::removeTimer(const QString &uuid)
{
    int ind = -1;
    for (int i = 0; i < m_timerList.size(); i++) {
        if (m_timerList[i]->uuid() == uuid) {
            ind = i;
            break;
        }
    }
    if (ind != -1) {
        this->remove(ind);
    }
}

void TimerModel::remove(int index)
{
    if ((index < 0) || (index >= m_timerList.count()))
        return;

    auto timer = m_timerList.at(index);

    Q_EMIT timerRemoved(timer->uuid());

    m_timerList.removeAt(index);
    timer->deleteLater();

    updateIndicators();
    save();
}

QList<Timer *> TimerModel::timerList() const
{
    return m_timerList;
}

Timer *TimerModel::timer(const QString &uuid) const
{
    for (Timer *timer : m_timerList) {
        if (timer->uuid() == uuid) {
            return timer;
        }
    }
    return nullptr;
}

QStringList TimerModel::timers() const
{
    QStringList ret;
    ret.reserve(m_timerList.size());

    // Filter out { } and - which are not allowed in DBus paths
    static QRegularExpression dbusfilter(QStringLiteral("[{}-]"));

    for (const Timer *timer : std::as_const(m_timerList)) {
        ret << timer->uuid().replace(dbusfilter, QString());
    }
    return ret;
}

void TimerModel::updateIndicators()
{
    qreal totalLength = 0;
    qreal totalElapsed = 0;
    for (Timer *timer : std::as_const(m_timerList)) {
        if (timer->running()) {
            totalLength += timer->length();
            totalElapsed += timer->elapsed();
        }

        // Notification could have been closed by the user.
        // It will only re-open when restarting the timer.
        if (m_notifications.contains(timer)) {
            updateNotification(timer);
        }
    }

    if (totalLength > 0) {
        if (!m_updateIndicatorsTimer.isActive()) {
            m_updateIndicatorsTimer.start();
        }

        m_unityLauncher->setProgress(totalElapsed / totalLength);
    } else {
        m_updateIndicatorsTimer.stop();

        m_unityLauncher->setProgress(qQNaN());
        Q_ASSERT(m_notifications.isEmpty());
    }
}

void TimerModel::maybeCreateNotification(Timer *timer)
{
    if (!timer->running()) {
        return;
    }

    if (m_notifications.value(timer)) {
        return;
    }

    if (KClockSettings::self()->timerNotification() == KClockSettings::EnumTimerNotification::Never
        || (KClockSettings::self()->timerNotification() == KClockSettings::EnumTimerNotification::WhenKClockNotRunning && m_kclockRunning)) {
        return;
    }

    auto *notification = new KNotification{u"timer"_s, KNotification::Persistent, timer};
    notification->setHint(u"resident"_s, true);
    connect(notification, &KNotification::closed, this, [this, timer] {
        m_notifications.remove(timer);
    });

    auto *defaultAction = notification->addDefaultAction(i18nc("@action:button Open kclock app", "Open Clock"));
    connect(defaultAction, &KNotificationAction::activated, notification, [notification] {
        launchKClock(notification->xdgActivationToken());
    });

    auto *plus1MinuteAction = notification->addAction(i18nc("@action:button Add minute to timer", "+1 Minute"));
    connect(plus1MinuteAction, &KNotificationAction::activated, timer, &Timer::addMinute);

    auto *resetAction = notification->addAction(i18nc("@action:button Reset timer", "Reset"));
    connect(resetAction, &KNotificationAction::activated, timer, &Timer::reset);

    auto *pauseAction = notification->addAction(i18nc("@action:button Pause timer", "Pause"));
    notification->setProperty("pauseAction", QVariant::fromValue(pauseAction));
    connect(pauseAction, &KNotificationAction::activated, timer, &Timer::toggleRunning);

    m_notifications.insert(timer, notification);
    updateNotification(timer);
    notification->sendEvent();
}

void TimerModel::updateNotification(Timer *timer)
{
    KNotification *notification = m_notifications.value(timer);
    Q_ASSERT(notification);

    const auto remaining = std::chrono::seconds{timer->length()} - std::chrono::seconds{timer->elapsed()};
    if (timer->running()) {
        notification->setTitle(timer->label());
    } else {
        notification->setTitle(i18nc("@title:notification Timer name (paused)", "%1 (Paused)", timer->label()));
    }

    notification->setText(Utilities::formatDuration(remaining));

    auto *pauseAction = notification->property("pauseAction").value<KNotificationAction *>();
    if (timer->running()) {
        pauseAction->setLabel(i18nc("@action:button Pause timer", "Pause"));
    } else {
        pauseAction->setLabel(i18nc("@action:button Resume timer", "Resume"));
    }
}

void TimerModel::onServiceRegistered(const QString &service)
{
    Q_UNUSED(service);
    m_kclockRunning = true;
    sendOrClearAllNotifications();
}

void TimerModel::onServiceUnregistered(const QString &service)
{
    Q_UNUSED(service);
    m_kclockRunning = false;
    sendOrClearAllNotifications();
}

void TimerModel::onTimerNotificationSettingChanged()
{
    sendOrClearAllNotifications();
}

void TimerModel::sendOrClearAllNotifications()
{
    if (KClockSettings::self()->timerNotification() == KClockSettings::EnumTimerNotification::Never
        || (KClockSettings::self()->timerNotification() == KClockSettings::EnumTimerNotification::WhenKClockNotRunning && m_kclockRunning)) {
        qCDebug(TIMERMODEL_DEBUG) << "Removing all timer notifications";
        for (auto *notification : std::as_const(m_notifications)) {
            disconnect(notification, &KNotification::closed, this, nullptr);
            notification->close();
        }
        m_notifications.clear();

    } else if (KClockSettings::self()->timerNotification() == KClockSettings::EnumTimerNotification::Always
               || (KClockSettings::self()->timerNotification() == KClockSettings::EnumTimerNotification::WhenKClockNotRunning && !m_kclockRunning)) {
        qCDebug(TIMERMODEL_DEBUG) << "Sending all timer notifications";

        for (Timer *timer : std::as_const(m_timerList)) {
            maybeCreateNotification(timer);
        }
    }
}

void TimerModel::launchKClock(const QString &xdgActivationToken)
{
    // Don't want to pull in KIO just for CommandLauncherJob...
    QProcess process;
    process.setProgram(QStandardPaths::findExecutable(QStringLiteral("kclock")));
    process.setArguments({QStringLiteral("--page"), QStringLiteral("Timers")});
    process.setProcessChannelMode(QProcess::ForwardedChannels);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert(QStringLiteral("XDG_ACTIVATION_TOKEN"), xdgActivationToken);
    process.setProcessEnvironment(env);

    process.startDetached();
}

#include "moc_timermodel.cpp"
