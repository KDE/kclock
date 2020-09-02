/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "timermodel.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KNotification>
#include <KSharedConfig>

#include <QDBusConnection>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>

#include "timer.h"
/* ~ TimerModel ~ */
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

void TimerModel::remove(QString uuid)
{
    int i = 0;
    for (auto timer : m_timerList) {
        if (timer->uuid().toString() == uuid)
            break;
        ++i;
    }
    this->remove(i);
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
