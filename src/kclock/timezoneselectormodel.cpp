/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
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
#include <QSettings>
#include <QTimeZone>

const QString TZ_CFG_GROUP = "Timezones";

TimeZoneSelectorModel::TimeZoneSelectorModel(QObject *parent)
    : QAbstractListModel(parent)
{
    auto config = KSharedConfig::openConfig();
    KConfigGroup timezoneGroup = config->group(TZ_CFG_GROUP);

    // add other configured time zones
    auto i{0};
    for (QByteArray id : QTimeZone::availableTimeZoneIds()) {
        bool show = timezoneGroup.readEntry(id.data(), false);
        m_list.append(std::make_tuple(QTimeZone(id), show));
        if (show) {
            m_selectedTimeZones.push_back(i);
        }
        i++;
    }

    connect(KclockFormat::instance(), &KclockFormat::timeChanged, this, &TimeZoneSelectorModel::update);
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

    QSettings settings;

    switch (role) {
    case NameRole:
        return std::get<0>(tuple).displayName(QDateTime::currentDateTime());
    case ShownRole:
        return std::get<1>(tuple);
    case IDRole:
        return std::get<0>(tuple).id();
    case ShortNameRole:
        return std::get<0>(tuple).displayName(QDateTime::currentDateTime(), QTimeZone::ShortName);
    case RelativeTimeRole: {
        int offset = std::get<0>(tuple).offsetFromUtc(QDateTime::currentDateTime()) - QTimeZone::systemTimeZone().offsetFromUtc(QDateTime::currentDateTime());
        offset /= 60; // convert to minutes

        QString hour = abs(offset) / 60 == 1 ? i18n("hour") : i18n("hours");

        if (offset > 0) {
            if (offset % 60) { // half an hour ahead
                return QVariant(i18n("%1 and a half hours ahead", offset / 60));
            } else { // full hours ahead
                return QVariant(i18n("%1 %2 ahead", offset / 60, hour));
            }
        } else if (offset < 0) {
            offset = abs(offset);
            if (offset % 60) { // half an hour behind
                return QVariant(i18n("%1 and a half hours behind", offset / 60));
            } else { // full hours behind
                return QVariant(i18n("%1 %2 behind", offset / 60, hour));
            }
        } else {
            return QVariant(i18n("Local time"));
        }
    }
    case TimeStringRole: {
        QDateTime time = QDateTime::currentDateTime();
        time = time.toTimeZone(std::get<0>(tuple));

        // apply 12 hour or 24 hour settings
        if (settings.value(QStringLiteral("Global/use24HourTime")).toBool()) {
            return time.time().toString("hh:mm");
        } else {
            return time.time().toString("h:mm ap");
        }
    }
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
    roles[RelativeTimeRole] = "relativeTime";
    roles[IDRole] = "id";
    return roles;
}
const std::vector<int> &TimeZoneSelectorModel::selectedTimeZones() const
{
    return m_selectedTimeZones;
}
void TimeZoneSelectorModel::update()
{
    QVector<int> roles = {TimeStringRole};
    Q_EMIT dataChanged(index(0), index(m_list.size() - 1), roles);
}

Qt::ItemFlags TimeZoneSelectorModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsEditable;
}

bool TimeZoneSelectorModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == ShownRole && value.type() == QVariant::Bool) {
        auto select = value.toBool();
        auto m_index = index.row();
        if (select) {
            std::get<1>(m_list[m_index]) = true;
            m_selectedTimeZones.push_back(m_index);
        } else {
            std::get<1>(m_list[m_index]) = false;
            auto end = std::remove(m_selectedTimeZones.begin(), m_selectedTimeZones.end(), m_index);
            m_selectedTimeZones.resize(std::distance(m_selectedTimeZones.begin(), end));
        }

        auto config = KSharedConfig::openConfig();
        KConfigGroup timezoneGroup = config->group(TZ_CFG_GROUP);
        timezoneGroup.writeEntry(std::get<0>(m_list[index.row()]).id().data(), value);

        Q_EMIT dataChanged(index, index, QVector<int>{ShownRole});
        return true;
    }
    return false;
}

TimeZoneFilterModel::TimeZoneFilterModel(TimeZoneSelectorModel *model, QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setSourceModel(model);
    setFilterRole(TimeZoneSelectorModel::IDRole);
}

TimeZoneViewModel::TimeZoneViewModel(TimeZoneSelectorModel *model, QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(model);
    setFilterRole(TimeZoneSelectorModel::ShownRole);
    setFilterFixedString(QStringLiteral("true"));
}
