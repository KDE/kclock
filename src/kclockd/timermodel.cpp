/*
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "timermodel.h"

#include "timer.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KNotification>
#include <KSharedConfig>

#include <QDBusConnection>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>

const QString TIMERS_CFG_GROUP = QStringLiteral("Timers"), TIMERS_CFG_KEY = QStringLiteral("timersList");

TimerModel *TimerModel::instance()
{
    static TimerModel *singleton = new TimerModel();
    return singleton;
}

TimerModel::TimerModel()
{
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
        m_timerList.append(new Timer(obj, this));
    }
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

void TimerModel::addTimer(int length, QString label, QString commandTimeout, bool running)
{
    auto *timer = new Timer(length, label, commandTimeout, running);
    m_timerList.append(timer);

    save();

    Q_EMIT timerAdded(timer->uuid());
}

void TimerModel::removeTimer(QString uuid)
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

    save();
}

QStringList TimerModel::timers() const
{
    QStringList ret;
    ret.reserve(m_timerList.size());

    // Filter out { } and - which are not allowed in DBus paths
    static QRegularExpression dbusfilter(QStringLiteral("[{}-]"));

    for (const Timer *timer : qAsConst(m_timerList)) {
        ret << timer->uuid().replace(dbusfilter, QString());
    }
    return ret;
}
