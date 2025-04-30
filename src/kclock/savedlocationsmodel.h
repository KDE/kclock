/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2021 Nicolas Fella <nicolas.fella@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QAbstractListModel>
#include <QSettings>
#include <QTimeZone>

#include <qqmlintegration.h>

class QJSEngine;
class QQmlEngine;

class SavedLocationsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    static SavedLocationsModel *instance();
    static SavedLocationsModel *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    enum Roles {
        NameRole = Qt::DisplayRole,
        TimeStringRole = Qt::UserRole + 1,
        RelativeTimeRole,
        RelativeDaysRole,
        CityRole,
        IdRole,
    };

    Q_INVOKABLE void removeLocation(int index);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

public Q_SLOTS:
    void load();

private:
    explicit SavedLocationsModel(QObject *parent = nullptr);

    std::vector<QTimeZone> m_timeZones;
    QSettings m_settings;
};
