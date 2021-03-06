/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
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
        m_timerList.append(new Timer(obj));
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
    group.writeEntry(TIMERS_CFG_KEY, QString(QJsonDocument(arr).toJson(QJsonDocument::Compact)));

    group.sync();
}

void TimerModel::addTimer(int length, QString label, bool running)
{
    auto *timer = new Timer(length, label, running);
    m_timerList.append(timer);

    save();

    Q_EMIT timerAdded(timer->uuid().toString());
}

void TimerModel::removeTimer(QString uuid)
{
    int ind = -1;
    for (int i = 0; i < m_timerList.size(); i++) {
        if (m_timerList[i]->uuid().toString() == uuid) {
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

    Q_EMIT timerRemoved(timer->uuid().toString());

    m_timerList.removeAt(index);
    timer->deleteLater();

    save();
}
