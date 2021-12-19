/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QTimer>

#include <array>
#include <tuple>

class WeekModel;
class KclockFormat : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentTime READ currentTime NOTIFY timeChanged)

public:
    explicit KclockFormat(QObject *parent = nullptr);

    static KclockFormat *instance()
    {
        static KclockFormat *singleton = new KclockFormat();
        return singleton;
    };

    QString currentTime();
    QString formatTimeString(int hours, int minutes);
    Q_INVOKABLE bool isChecked(int dayIndex, int daysOfWeek);

private Q_SLOTS:
    void updateCurrentTime();

Q_SIGNALS:
    void timeChanged();

private:
    void startTimer(); // basic settings for updating time display

    QTimer *m_timer;
    WeekModel *m_weekModel;
    QString m_currentTime;
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
