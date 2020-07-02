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

#include <KLocalizedString>
#include <KNotification>
#include <KSharedConfig>
#include <KConfigGroup>

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <QtGlobal>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

/* ~ Timer ~ */

Timer::Timer(QObject *parent, int length, int elapsed, QString label, bool running)
{
    length_ = length;
    elapsed_ = elapsed;
    label_ = label;
    running_ = running;
    finished_ = false;
    justCreated_ = true;
    
    connect(this, &Timer::propertyChanged, this, []{TimerModel::inst()->saveRequested = true;}, Qt::UniqueConnection);
}

Timer::Timer(const QJsonObject &obj)
{
    length_ = obj["length"].toInt();
    elapsed_ = obj["elapsed"].toInt();
    label_ = obj["label"].toString();
    running_ = obj["running"].toBool();
    finished_ = obj["finished"].toBool();
    justCreated_ = false;
    
    connect(this, &Timer::propertyChanged, this, []{TimerModel::inst()->saveRequested = true;}, Qt::UniqueConnection);
}

QJsonObject Timer::serialize()
{
    QJsonObject obj;
    obj["length"] = length_;
    obj["elapsed"] = elapsed_;
    obj["label"] = label_;
    obj["running"] = running_;
    obj["finished"] = finished_;
    return obj;
}

void Timer::updateTimer(qint64 duration)
{
    if (running_) {
        elapsed_ += duration;
        
        // if the timer has finished
        if (elapsed_ >= length_) {
            elapsed_ = length_;
            running_ = false;
            finished_ = true;
            
            qDebug("Timer finished, sending notification...");

            KNotification *notif = new KNotification("timerFinished");
            notif->setIconName("kclock");
            notif->setTitle(i18n("Timer complete"));
            notif->setText(i18n("Your timer has finished!"));
            notif->setDefaultAction(i18n("View"));
            notif->setUrgency(KNotification::HighUrgency);
            notif->setFlags(KNotification::NotificationFlag::LoopSound | KNotification::NotificationFlag::Persistent);
            notif->sendEvent();
        }
        
        emit propertyChanged();
    }
}

void Timer::toggleRunning()
{
    if (finished_) {
        elapsed_ = 0;
        running_ = true;
        finished_ = false;
    } else {
        running_ = !running_;
    }
    emit propertyChanged();
}

void Timer::reset()
{
    finished_ = false;
    running_ = false;
    elapsed_ = 0;
    emit propertyChanged();
}


/* ~ TimerModel ~ */
const int TIMER_CHECK_LENGTH = 16; // milliseconds
const QString TIMERS_CFG_GROUP = "Timers", TIMERS_CFG_KEY = "timersList";

TimerModel::TimerModel(QObject *parent)
{
    load();
    
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&TimerModel::updateTimerLoop));
    timer->start(TIMER_CHECK_LENGTH);
    
    // slow down saves to max once per second
    saveTimer = new QTimer(this);
    connect(saveTimer, &QTimer::timeout, this, [this]{
        if (saveRequested) {
            save();
            saveRequested = false;
        }
    });
    saveTimer->start(1000);
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

void TimerModel::updateTimerLoop()
{
    for (auto *timer : timerList) 
        timer->updateTimer(TIMER_CHECK_LENGTH); 
}

int TimerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return timerList.size();
}

int TimerModel::count()
{
    return timerList.size();
}

QVariant TimerModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

void TimerModel::addNew()
{
    insert(count(), new Timer(this));
}

void TimerModel::insert(int index, Timer *timer)
{
    if ((index < 0) || (index > timerList.count())) 
        return;
    
    QQmlEngine::setObjectOwnership(timer, QQmlEngine::CppOwnership);
    emit beginInsertRows(QModelIndex(), index, index);
    timerList.insert(index, timer);
    emit endInsertRows();
    
    save();
}

void TimerModel::remove(int index)
{
    if ((index < 0) || (index >= timerList.count()))
        return;
    
    emit beginRemoveRows(QModelIndex(), index, index);
    auto timer = timerList.at(index);
    timerList.removeAt(index);
    delete timer;
    emit endRemoveRows();
    
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
