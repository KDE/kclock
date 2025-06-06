/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "kclockformat.h"

#include <QAbstractListModel>
#include <QSet>
#include <QSortFilterProxyModel>
#include <QTimeZone>
#include <QTimer>

#include <qqmlintegration.h>

class QJSEngine;
class QQmlEngine;

class AddLocationModel : public QAbstractListModel
{
    Q_OBJECT

public:
    static AddLocationModel *instance();

    explicit AddLocationModel(QObject *parent = nullptr);

    enum Roles {
        CityRole = Qt::DisplayRole,
        CountryRole = Qt::DisplayRole + 1,
        TimeZoneRole = Qt::UserRole + 0,
        CurrentTimeRole = Qt::UserRole + 1,
        AddedRole = Qt::UserRole + 2,
        IdRole
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

public Q_SLOTS:
    void load();
    void updateAddedLocations();

private:
    QList<QTimeZone> m_list;
    QSet<QByteArray> m_addedLocations;
};

class AddLocationSearchModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)

public:
    static AddLocationSearchModel *instance();
    static AddLocationSearchModel *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    QString query() const;
    void setQuery(const QString &query);
    Q_SIGNAL void queryChanged(const QString &query);

    Q_INVOKABLE void addLocation(int row);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    explicit AddLocationSearchModel(QObject *parent = nullptr);

    QString m_query;
};
