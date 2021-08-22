/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KCLOCK_TIMEZONESELECTORMODEL_H
#define KCLOCK_TIMEZONESELECTORMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QTimeZone>
#include <QTimer>

class TimeZoneSelectorModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit TimeZoneSelectorModel(QObject *parent = nullptr);

    enum Roles {
        NameRole = Qt::DisplayRole,
        ShownRole = Qt::UserRole + 0,
        OffsetRole = Qt::UserRole + 1,
        ShortNameRole = Qt::UserRole + 2,
        TimeStringRole,
        RelativeTimeRole,
        IDRole
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

    const std::vector<int> &selectedTimeZones() const;

public Q_SLOTS:
    Q_INVOKABLE void update();
Q_SIGNALS:
    void selectedTimeZoneChanged();

private:
    QList<std::tuple<QTimeZone, bool>> m_list;
    std::vector<int> m_selectedTimeZones;
};

class TimeZoneFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit TimeZoneFilterModel(TimeZoneSelectorModel *model, QObject *parent = nullptr);
};

class TimeZoneViewModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit TimeZoneViewModel(TimeZoneSelectorModel *model, QObject *parent = nullptr);
};

#endif // KCLOCK_TIMEZONESELECTORMODEL_H
