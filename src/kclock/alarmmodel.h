/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *                Han Young <hanyoung@protonmail.com>
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

#pragma once

#include <QAbstractListModel>
#include <QObject>

#include "alarmmodelinterface.h"
class Alarm;
class AlarmModel : public QAbstractListModel
{
    Q_OBJECT
public:
    static AlarmModel *instance()
    {
        static AlarmModel *singleton = new AlarmModel();
        return singleton;
    }

    enum {
        NameRole = Qt::DisplayRole,
        EnabledRole = Qt::UserRole + 1,
        HoursRole,
        MinutesRole,
        DaysOfWeekRole,
        AlarmRole,
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void updateUi();

    Q_INVOKABLE void remove(int index);

    Q_INVOKABLE void addAlarm(int hours, int minutes, int daysOfWeek, QString name, QString ringtonePath = 0); // in 24 hours units, ringTone could be chosen from a list

    Q_INVOKABLE QString timeToRingFormated(int hours, int minutes, int daysOfWeek); // for new alarm use
private Q_SLOTS:
    void addAlarm(QString uuid);
    void removeAlarm(QString uuid);

private:
    org::kde::kclock::AlarmModel *m_interface;

    explicit AlarmModel(QObject *parent = nullptr);
    QList<Alarm *> alarmsList;
};
