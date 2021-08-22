/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2021 Nicolas Fella <nicolas.fella@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "savedtimezonesmodel.h"

#include <QDebug>
#include <QTimeZone>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include "kclockformat.h"

const QString TZ_CFG_GROUP = "Timezones";

SavedTimeZonesModel::SavedTimeZonesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    auto config = KSharedConfig::openConfig();
    KConfigGroup timezoneGroup = config->group(TZ_CFG_GROUP);

    for (const QString &timezoneId : timezoneGroup.keyList()) {
        QTimeZone z(timezoneId.toUtf8());
        m_timeZones.push_back(z);
    }
    connect(KclockFormat::instance(), &KclockFormat::timeChanged, this, [this] {
        QVector<int> roles = {TimeStringRole};
        Q_EMIT dataChanged(index(0), index(m_timeZones.size() - 1), roles);
    });
}

int SavedTimeZonesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_timeZones.size();
}

QVariant SavedTimeZonesModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();

    switch (role) {
    case NameRole:
        return m_timeZones[row].displayName(QDateTime::currentDateTime());
    case TimeStringRole: {
        QDateTime time = QDateTime::currentDateTime();
        time = time.toTimeZone(m_timeZones[row]);

        // apply 12 hour or 24 hour settings
        if (m_settings.value(QStringLiteral("Global/use24HourTime")).toBool()) {
            return time.time().toString("hh:mm");
        } else {
            return time.time().toString("h:mm ap");
        }
    }
    case RelativeTimeRole: {
        int offset = m_timeZones[row].offsetFromUtc(QDateTime::currentDateTime()) - QTimeZone::systemTimeZone().offsetFromUtc(QDateTime::currentDateTime());
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
    }

    return {};
}

QHash<int, QByteArray> SavedTimeZonesModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {TimeStringRole, "timeString"},
        {RelativeTimeRole, "relativeTime"},
    };
}
