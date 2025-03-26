/*
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "timermodel.h"

#include "timer.h"
#include "unitylauncher.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KNotification>
#include <KSharedConfig>

#include <QDBusConnection>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QtNumeric>

#include <chrono>

using namespace std::literals::chrono_literals;

const QString TIMERS_CFG_GROUP = QStringLiteral("Timers"), TIMERS_CFG_KEY = QStringLiteral("timersList");

TimerModel *TimerModel::instance()
{
    static TimerModel *singleton = new TimerModel();
    return singleton;
}

TimerModel::TimerModel()
    : QObject(nullptr)
    , m_unityLauncher(new UnityLauncher(this))
{
    // Match what app uses, reduces a possible 1s gap between task bar and app
    // when one or the other ever so slightly misses the second changing.
    m_updateUnityLauncherTimer.setInterval(250ms);
    m_updateUnityLauncherTimer.callOnTimeout(this, &TimerModel::updateUnityLauncher);

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
        connectTimer(timer);
        m_timerList.append(timer);
    }
    updateUnityLauncher();
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

    save();
    updateUnityLauncher();

    Q_EMIT timerAdded(timer->uuid());
}

void TimerModel::connectTimer(Timer *timer)
{
    connect(timer, &Timer::runningChanged, this, &TimerModel::updateUnityLauncher);
    connect(timer, &Timer::lengthChanged, this, &TimerModel::updateUnityLauncher);
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

    updateUnityLauncher();
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

void TimerModel::updateUnityLauncher()
{
    qreal totalLength = 0;
    qreal totalElapsed = 0;
    for (const Timer *timer : std::as_const(m_timerList)) {
        if (!timer->running()) {
            continue;
        }

        totalLength += timer->length();
        totalElapsed += timer->elapsed();
    }

    if (totalLength > 0) {
        m_unityLauncher->setProgress(totalElapsed / totalLength);
        if (!m_updateUnityLauncherTimer.isActive()) {
            m_updateUnityLauncherTimer.start();
        }
    } else {
        m_unityLauncher->setProgress(qQNaN());
        m_updateUnityLauncherTimer.stop();
    }
}

#include "moc_timermodel.cpp"
