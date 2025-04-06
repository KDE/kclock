/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "addlocationmodel.h"

#include "savedlocationsmodel.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <QDebug>
#include <QQmlEngine>

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
    m_addedLocations.clear();

    auto config = KSharedConfig::openConfig();
    KConfigGroup timezoneGroup = config->group(TZ_CFG_GROUP);

    const auto timeZoneIds = QTimeZone::availableTimeZoneIds();
    m_list.reserve(timeZoneIds.count());
    for (const QByteArray &id : timeZoneIds) {
        m_list.append(QTimeZone(id));

        const bool shown = timezoneGroup.readEntry(id.constData(), false);
        if (shown) {
            m_addedLocations.insert(id);
        }
    }

    endResetModel();
}

void AddLocationModel::updateAddedLocations()
{
    const KConfigGroup timezoneGroup = KSharedConfig::openConfig()->group(TZ_CFG_GROUP);
    for (int i = 0; i < m_list.count(); ++i) {
        const QTimeZone &timeZone = m_list.at(i);
        const QByteArray timeZoneId = timeZone.id();

        const bool wasShown = m_addedLocations.contains(timeZoneId);
        const bool shown = timezoneGroup.readEntry(timeZoneId.constData(), false);

        if (shown == wasShown) {
            continue;
        }

        if (shown && !wasShown) {
            m_addedLocations.insert(timeZoneId);
        } else if (!shown && wasShown) {
            m_addedLocations.remove(timeZoneId);
        }

        const QModelIndex changedIndex = index(i, 0);
        Q_EMIT dataChanged(changedIndex, changedIndex, {AddLocationModel::AddedRole});
    }
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

    const QTimeZone &tz = m_list.at(index.row());
    const QByteArray id = tz.id().replace("_", " ");

    switch (role) {
    case CityRole: {
        const int lastSlashIdx = id.lastIndexOf('/');
        return id.mid(lastSlashIdx + 1);
    }
    case CountryRole:
        return QLocale::territoryToString(tz.territory());
    case TimeZoneRole:
        return tz.displayName(QDateTime::currentDateTime(), QTimeZone::ShortName);
    case IdRole:
        return id;
    case AddedRole:
        return m_addedLocations.contains(id);
    case CurrentTimeRole: {
        QDateTime time = QDateTime::currentDateTime();
        time = time.toTimeZone(tz);
        return KclockFormat::instance()->formatTimeString(time.time().hour(), time.time().minute());
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

AddLocationSearchModel *AddLocationSearchModel::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine);
    Q_UNUSED(jsEngine);
    auto *model = instance();
    QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);
    return model;
}

AddLocationSearchModel::AddLocationSearchModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setSourceModel(AddLocationModel::instance());
    sort(0);
}

QString AddLocationSearchModel::query() const
{
    return m_query;
}

void AddLocationSearchModel::setQuery(const QString &query)
{
    if (m_query == query) {
        return;
    }

    m_query = query;
    invalidateFilter();
    Q_EMIT queryChanged(query);
}

void AddLocationSearchModel::addLocation(int row)
{
    auto config = KSharedConfig::openConfig();
    KConfigGroup timezoneGroup = config->group(TZ_CFG_GROUP);
    const QModelIndex locationIndex = index(row, 0);
    const QString ianaId = locationIndex.data(AddLocationModel::IdRole).toString().replace(QStringLiteral(" "), QStringLiteral("_"));
    timezoneGroup.writeEntry(ianaId, true);
    config->sync();

    AddLocationModel::instance()->updateAddedLocations();
    SavedLocationsModel::instance()->load();
}

bool AddLocationSearchModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!m_query.isEmpty()) {
        const QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);

        const QString timezoneId = sourceIndex.data(AddLocationModel::IdRole).toString();
        const QString city = sourceIndex.data(AddLocationModel::CityRole).toString();
        const QString country = sourceIndex.data(AddLocationModel::CountryRole).toString();
        if (!timezoneId.contains(m_query, Qt::CaseInsensitive) && !city.contains(m_query, Qt::CaseInsensitive)
            && !country.contains(m_query, Qt::CaseInsensitive)) {
            return false;
        }
    }

    return true;
}

bool AddLocationSearchModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    return QVariant::compare(sourceModel()->data(left, AddLocationModel::CityRole), sourceModel()->data(right, AddLocationModel::CityRole))
        == QPartialOrdering::Less;
}

#include "moc_addlocationmodel.cpp"
