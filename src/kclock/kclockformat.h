/*
 * Copyright 2020   Han Young <hanyoung@protonmail.com>
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
#ifndef KCLOCKFORMAT_H
#define KCLOCKFORMAT_H

#include <QAbstractListModel>
#include <QObject>
#include <array>
#include <tuple>
class QTimer;
class WeekModel;
class KclockFormat : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentTime READ currentTime NOTIFY timeChanged)
    Q_PROPERTY(int seconds READ seconds NOTIFY secondChanged)
    Q_PROPERTY(int minutes READ minutes NOTIFY timeChanged)
    Q_PROPERTY(int hours READ hours NOTIFY hourChanged)
public:
    explicit KclockFormat(QObject *parent = nullptr);
    Q_INVOKABLE QString formatTimeString(int hours, int minutes);
    Q_INVOKABLE bool isChecked(int dayIndex, int daysOfWeek);
    QString currentTime()
    {
        return m_currentTime;
    };
    int seconds()
    {
        return m_minutesCounter;
    }
    int minutes()
    {
        return m_hoursCounter;
    }
    int hours()
    {
        return m_hours;
    }

private Q_SLOTS:
    void updateTime();
Q_SIGNALS:
    void timeChanged();
    void startDayChanged();
    void secondChanged();
    void hourChanged();

private:
    void startTimer(); // basic settings for updating time display

    QTimer *m_timer;
    WeekModel *m_weekModel;
    QString m_currentTime;

    int m_hours, m_hoursCounter = 0, m_minutesCounter = 0;
};

using weekListItem = std::array<std::tuple<QString, int>, 7>;
class WeekModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum { NameRole = Qt::DisplayRole, FlagRole = Qt::UserRole + 1 };
    explicit WeekModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    weekListItem m_listItem;
};

#endif // KCLOCKFORMAT_H
