/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2021 Nicolas Fella <nicolas.fella@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "savedlocationsmodel.h"

#include <QDebug>
#include <QTimeZone>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include "addlocationmodel.h"
#include "kclockformat.h"
#include "settingsmodel.h"
#include "utilmodel.h"

const QString TZ_CFG_GROUP = QStringLiteral("Timezones");

SavedLocationsModel *SavedLocationsModel::instance()
{
    static SavedLocationsModel *singleton = new SavedLocationsModel;
    return singleton;
}

SavedLocationsModel::SavedLocationsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    load();

    connect(KclockFormat::instance(), &KclockFormat::timeChanged, this, [this] {
        Q_EMIT dataChanged(index(0), index(m_timeZones.size() - 1), {TimeStringRole});
    });
}

Q_INVOKABLE void SavedLocationsModel::removeLocation(int index)
{
    auto config = KSharedConfig::openConfig();
    KConfigGroup timezoneGroup = config->group(TZ_CFG_GROUP);
    QString ianaId = data(this->index(index, 0), SavedLocationsModel::IdRole).toString().replace(QStringLiteral(" "), QStringLiteral("_"));
    timezoneGroup.deleteEntry(ianaId);
    config->sync();

    AddLocationModel::instance()->load();

    beginRemoveRows(QModelIndex(), index, index);
    m_timeZones.erase(m_timeZones.begin() + index);
    endRemoveRows();
}

void SavedLocationsModel::load()
{
    beginResetModel();

    m_timeZones.clear();

    auto config = KSharedConfig::openConfig();
    KConfigGroup timezoneGroup = config->group(TZ_CFG_GROUP);

    for (const QString &timezoneId : timezoneGroup.keyList()) {
        m_timeZones.push_back(QTimeZone{timezoneId.toUtf8()});
    }

    endResetModel();
}

int SavedLocationsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_timeZones.size();
}

QVariant SavedLocationsModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();

    switch (role) {
    case NameRole: {
        auto split = m_timeZones[row].displayName(QDateTime::currentDateTime()).split(QLatin1Char('/'));
        return split[split.length() - 1];
    }
    case TimeStringRole: {
        QDateTime time = QDateTime::currentDateTime();
        time = time.toTimeZone(m_timeZones[row]);

        // apply 12 hour or 24 hour settings
        if (UtilModel::instance()->use24HourTime()) {
            return time.time().toString(QStringLiteral("hh:mm"));
        } else {
            return time.time().toString(QStringLiteral("h:mm ap"));
        }
    }
    case RelativeTimeRole: {
        int offset = m_timeZones[row].offsetFromUtc(QDateTime::currentDateTime()) - QTimeZone::systemTimeZone().offsetFromUtc(QDateTime::currentDateTime());
        offset /= 60; // convert to minutes

        bool isHour = abs(offset) / 60 == 1;

        if (offset > 0) {
            if (offset % 60) { // half an hour ahead
                return isHour ? QVariant(i18n("%1 hour and 30 minutes ahead", offset / 60)) : QVariant(i18n("%1 hours and 30 minutes ahead", offset / 60));
            } else { // full hours ahead
                return isHour ? QVariant(i18n("%1 hour ahead", offset / 60)) : QVariant(i18n("%1 hours ahead", offset / 60));
            }
        } else if (offset < 0) {
            offset = abs(offset);
            if (offset % 60) { // half an hour behind
                return isHour ? QVariant(i18n("%1 hour and 30 minutes behind", offset / 60)) : QVariant(i18n("%1 hours and 30 minutes behind", offset / 60));
            } else { // full hours behind
                return isHour ? QVariant(i18n("%1 hour behind", offset / 60)) : QVariant(i18n("%1 hours behind", offset / 60));
            }
        } else {
            return QVariant(i18n("Local time"));
        }
    }
    case CityRole: {
        auto split = m_timeZones[row].id().replace("_", " ").split('/');
        return split[split.length() - 1];
    }
    case IdRole: {
        return m_timeZones[row].id().replace("_", " ");
    }
    }

    return {};
}

QHash<int, QByteArray> SavedLocationsModel::roleNames() const
{
    return {{NameRole, "name"}, {TimeStringRole, "timeString"}, {RelativeTimeRole, "relativeTime"}, {CityRole, "city"}, {IdRole, "id"}};
}
