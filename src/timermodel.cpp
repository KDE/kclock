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

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QtGlobal>

#include "timer.h"
/* ~ TimerModel ~ */
const int TIMER_CHECK_LENGTH = 16; // milliseconds
const QString TIMERS_CFG_GROUP = "Timers", TIMERS_CFG_KEY = "timersList";

TimerModel::TimerModel(QObject *parent)
{
    load();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&TimerModel::updateTimerLoop));
    timer->setInterval(TIMER_CHECK_LENGTH);
    updateTimerStatus(); // start timer loop if necessary

    // slow down saves to max once per second
    saveTimer = new QTimer(this);
    saveTimer->setSingleShot(true);
    saveTimer->setInterval(1000);
    connect(saveTimer, &QTimer::timeout, this, [this] { save(); });
}

void TimerModel::load()
{
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(TIMERS_CFG_GROUP);
    QJsonDocument doc = QJsonDocument::fromJson(group.readEntry(TIMERS_CFG_KEY, "{}").toUtf8());
    for (QJsonValueRef r : doc.array()) {
        QJsonObject obj = r.toObject();
        timerList.append(new Timer(obj));
    }
}

void TimerModel::save()
{
    QJsonArray arr;
    for (auto timer : timerList) {
        arr.push_back(timer->serialize());
    }
    QJsonObject obj;
    obj["list"] = arr;

    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(TIMERS_CFG_GROUP);
    group.writeEntry(TIMERS_CFG_KEY, QString(QJsonDocument(arr).toJson(QJsonDocument::Compact)));

    group.sync();
}

void TimerModel::requestSave()
{
    if (!saveTimer->isActive()) {
        saveTimer->start();
    }
}

void TimerModel::updateTimerLoop()
{
    for (auto *timer : timerList)
        timer->updateTimer(TIMER_CHECK_LENGTH);
}

void TimerModel::updateTimerStatus()
{
    // stop timer if all timers are inactive
    if (areTimersInactive() && timer->isActive()) {
        timer->stop();
    } else if (!areTimersInactive() && !timer->isActive()) {
        timer->start();
    }
}

bool TimerModel::areTimersInactive()
{
    for (auto *timer : timerList) {
        if (timer->running()) {
            return false;
        }
    }
    return true;
}

void TimerModel::remove(int index)
{
    if ((index < 0) || (index >= timerList.count()))
        return;

    auto timer = timerList.at(index);
    timerList.removeAt(index);
    delete timer;

    save();
}

Timer *TimerModel::get(int index)
{
    if ((index < 0) || (index >= timerList.count()))
        return {};

    return timerList.at(index);
}

void TimerModel::move(int oldIndex, int newIndex)
{
    if (oldIndex < 0 || oldIndex >= timerList.count() || newIndex < 0 || newIndex >= timerList.count())
        return;

    timerList.move(oldIndex, newIndex);
}
