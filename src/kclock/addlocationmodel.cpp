/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "addlocationmodel.h"

#include "kclockformat.h"
#include "savedlocationsmodel.h"
#include "utilmodel.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <QDebug>
#include <QTimeZone>

const QString TZ_CFG_GROUP = QStringLiteral("Timezones");

AddLocationModel *AddLocationModel::instance()
{
    static auto singleton = new AddLocationModel;
    return singleton;
}

AddLocationModel::AddLocationModel(QObject *parent)
    : QAbstractListModel(parent)
{
    load();

    connect(KclockFormat::instance(), &KclockFormat::timeChanged, this, [this] {
        Q_EMIT dataChanged(index(0), index(m_list.size() - 1), {CurrentTimeRole});
    });
}

void AddLocationModel::load()
{
    beginResetModel();

    m_list.clear();

    auto config = KSharedConfig::openConfig();
    KConfigGroup timezoneGroup = config->group(TZ_CFG_GROUP);

    // add other configured time zones
    for (QByteArray id : QTimeZone::availableTimeZoneIds()) {
        bool shown = timezoneGroup.readEntry(id.data(), false);
        m_list.append(QTimeZone(id));

        if (shown) {
            m_addedLocations.insert(QString::fromStdString(id.toStdString()));
        }
    }

    endResetModel();
}

int AddLocationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.count();
}

QVariant AddLocationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto tz = m_list[index.row()];
    QString id = QString::fromStdString(tz.id().replace("_", " ").toStdString());

    switch (role) {
    case CityRole: {
        auto split = id.split(QLatin1Char('/'));
        return split[split.size() - 1];
    }
    case CountryRole:
        return QLocale::countryToString(tz.country());
    case TimeZoneRole:
        return tz.displayName(QDateTime::currentDateTime(), QTimeZone::ShortName);
    case IdRole:
        return id;
    case AddedRole:
        return m_addedLocations.find(id) != m_addedLocations.end();
    case CurrentTimeRole: {
        QDateTime time = QDateTime::currentDateTime();
        time = time.toTimeZone(tz);
        return KclockFormat().formatTimeString(time.time().hour(), time.time().minute());
    }
    }
    return QVariant();
}

QHash<int, QByteArray> AddLocationModel::roleNames() const
{
    return {{CityRole, "city"}, {CountryRole, "country"}, {TimeZoneRole, "timeZone"}, {IdRole, "id"}, {AddedRole, "added"}, {CurrentTimeRole, "currentTime"}};
}

Qt::ItemFlags AddLocationModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsEditable;
}

AddLocationSearchModel *AddLocationSearchModel::instance()
{
    static auto singleton = new AddLocationSearchModel;
    return singleton;
}

AddLocationSearchModel::AddLocationSearchModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setSourceModel(AddLocationModel::instance());
    setFilterRole(AddLocationModel::IdRole);
    sort(0);
}

void AddLocationSearchModel::addLocation(int index)
{
    auto config = KSharedConfig::openConfig();
    KConfigGroup timezoneGroup = config->group(TZ_CFG_GROUP);
    QString ianaId = data(this->index(index, 0), AddLocationModel::IdRole).toString().replace(QStringLiteral(" "), QStringLiteral("_"));
    timezoneGroup.writeEntry(ianaId, true);
    config->sync();

    AddLocationModel::instance()->load();
    SavedLocationsModel::instance()->load();
}

bool AddLocationSearchModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    return sourceModel()->data(left, AddLocationModel::CityRole) < sourceModel()->data(right, AddLocationModel::CityRole);
}
