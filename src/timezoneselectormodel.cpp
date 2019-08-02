/*
 * Copyright 2019  Nick Reitemeyer <nick.reitemeyer@web.de>
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

#include <QTimeZone>
#include <QDebug>
#include <KSharedConfig>
#include <KConfigGroup>
#include "timezoneselectormodel.h"

TimeZoneSelectorModel::TimeZoneSelectorModel(QObject* parent) : QAbstractListModel(parent)
{
    auto config = KSharedConfig::openConfig();
    KConfigGroup timezoneGroup = config->group("Timezones");
    for (QByteArray id : QTimeZone::availableTimeZoneIds()) {
        bool show = timezoneGroup.readEntry(id.data(), false);
        mList.append(std::make_tuple(QTimeZone(id), show));
    }
    mTimer.setInterval(1000);
    connect(&mTimer, &QTimer::timeout, this, &TimeZoneSelectorModel::update);
    mTimer.start(1000);
}


int TimeZoneSelectorModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mList.count();
}

QVariant TimeZoneSelectorModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();
    auto tuple = mList[index.row()];
    switch(role) {
    case NameRole:
        return std::get<0>(tuple).displayName(QDateTime::currentDateTime());
    case ShownRole:
        return std::get<1>(tuple);
    case IDRole:
        return std::get<0>(tuple).id();
    case ShortNameRole:
        return std::get<0>(tuple).displayName(QDateTime::currentDateTime(), QTimeZone::ShortName);
    case TimeStringRole:
        QDateTime time = QDateTime::currentDateTime();
        time = time.toTimeZone(std::get<0>(tuple));
        return time.time().toString("hh:mm:ss");
    }
    return QVariant();
}

QHash<int, QByteArray> TimeZoneSelectorModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ShownRole] = "shown";
    roles[OffsetRole] = "offset";
    roles[ShortNameRole] = "shortName";
    roles[TimeStringRole] = "timeString";
    roles[IDRole] = "id";
    return roles;
}

void TimeZoneSelectorModel::update()
{
    QVector<int> roles = { TimeStringRole };
    emit dataChanged(index(0), index(mList.size() - 1), roles);
}

Qt::ItemFlags TimeZoneSelectorModel::flags(const QModelIndex& index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsEditable;
}

bool TimeZoneSelectorModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(index.isValid() && role == ShownRole && value.type() == QVariant::Bool) {
        std::get<1>(mList[index.row()]) = value.toBool();
        auto config = KSharedConfig::openConfig();
        KConfigGroup timezoneGroup = config->group("Timezones");
        timezoneGroup.writeEntry(std::get<0>(mList[index.row()]).id().data(), value);
        emit dataChanged(index, index, QVector<int> { ShownRole });
        return true;
    }
    return false;
}

TimeZoneFilterModel::TimeZoneFilterModel(TimeZoneSelectorModel *model, QObject *parent) : QSortFilterProxyModel (parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setSourceModel(model);
    setFilterRole(TimeZoneSelectorModel::IDRole);
}

TimeZoneViewModel::TimeZoneViewModel(TimeZoneSelectorModel *model, QObject *parent) : QSortFilterProxyModel(parent)
{
    setSourceModel(model);
    setFilterRole(TimeZoneSelectorModel::ShownRole);
    setFilterFixedString("true");
}
