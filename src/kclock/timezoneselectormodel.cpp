/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "timezoneselectormodel.h"

#include "kclockformat.h"
#include "utilmodel.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <QDebug>
#include <QTimeZone>

const QString TZ_CFG_GROUP = QStringLiteral("Timezones");

TimeZoneSelectorModel::TimeZoneSelectorModel(QObject *parent)
    : QAbstractListModel(parent)
{
    auto config = KSharedConfig::openConfig();
    KConfigGroup timezoneGroup = config->group(TZ_CFG_GROUP);

    // add other configured time zones
    for (QByteArray id : QTimeZone::availableTimeZoneIds()) {
        bool show = timezoneGroup.readEntry(id.data(), false);
        m_list.append(std::make_tuple(QTimeZone(id), show));
    }

    connect(KclockFormat::instance(), &KclockFormat::timeChanged, this, [this] {
        Q_EMIT dataChanged(index(0), index(m_list.size() - 1), {CurrentTimeRole});
    });
}

int TimeZoneSelectorModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.count();
}

QVariant TimeZoneSelectorModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    auto tuple = m_list[index.row()];

    switch (role) {
    case NameRole:
        return std::get<0>(tuple).displayName(QDateTime::currentDateTime());
    case ShownRole:
        return std::get<1>(tuple);
    case IDRole:
        return std::get<0>(tuple).id().replace("_", " ");
    case ShortNameRole:
        return std::get<0>(tuple).displayName(QDateTime::currentDateTime(), QTimeZone::ShortName);
    case CurrentTimeRole:
        QDateTime time = QDateTime::currentDateTime();
        time = time.toTimeZone(std::get<0>(tuple));
        return KclockFormat().formatTimeString(time.time().hour(), time.time().minute());
    }
    return QVariant();
}

QHash<int, QByteArray> TimeZoneSelectorModel::roleNames() const
{
    return {{NameRole, "name"}, {ShownRole, "shown"}, {OffsetRole, "offset"}, {ShortNameRole, "shortName"}, {IDRole, "id"}, {CurrentTimeRole, "currentTime"}};
}

Qt::ItemFlags TimeZoneSelectorModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsEditable;
}

bool TimeZoneSelectorModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == ShownRole && value.type() == QVariant::Bool) {
        auto selected = value.toBool();
        std::get<1>(m_list[index.row()]) = selected;

        auto config = KSharedConfig::openConfig();
        KConfigGroup timezoneGroup = config->group(TZ_CFG_GROUP);
        timezoneGroup.writeEntry(std::get<0>(m_list[index.row()]).id().data(), value);
        Q_EMIT dataChanged(index, index, QVector<int>{ShownRole});
        UtilModel::instance()->setSelectedTimezone(std::get<0>(m_list[index.row()]).id(), selected);
        return true;
    }
    return false;
}

TimeZoneFilterModel::TimeZoneFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setSourceModel(new TimeZoneSelectorModel(this));
    setFilterRole(TimeZoneSelectorModel::IDRole);
}
