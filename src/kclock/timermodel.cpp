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

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtGlobal>

/* ~ TimerModel ~ */
const QString TIMERS_CFG_GROUP = "Timers", TIMERS_CFG_KEY = "timersList";

TimerModel::TimerModel(QObject *parent)
{
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
}

void TimerModel::remove(int index)
{
}

Timer *TimerModel::get(int index)
{
    if ((index < 0) || (index >= timerList.count()))
        return {};

    return timerList.at(index);
}
