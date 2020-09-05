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

#ifndef TIMEZONESELECTORMODEL_H
#define TIMEZONESELECTORMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QTimeZone>
#include <QTimer>

class TimeZoneSelectorModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit TimeZoneSelectorModel(QObject *parent = nullptr);

    enum Roles { NameRole = Qt::DisplayRole, ShownRole = Qt::UserRole + 0, OffsetRole = Qt::UserRole + 1, ShortNameRole = Qt::UserRole + 2, TimeStringRole, RelativeTimeRole, IDRole };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

public Q_SLOTS:
    void update();

private:
    QList<std::tuple<QTimeZone, bool>> m_list;
    QTimer m_timer;
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

#endif
