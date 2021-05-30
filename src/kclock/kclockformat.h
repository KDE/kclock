/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KCLOCKFORMAT_H
#define KCLOCKFORMAT_H

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
    Q_PROPERTY(int seconds READ seconds NOTIFY secondChanged)
    Q_PROPERTY(int minutes READ minutes NOTIFY timeChanged)
    Q_PROPERTY(int hours READ hours NOTIFY hourChanged)
public:
    explicit KclockFormat(QObject *parent = nullptr);

    static KclockFormat *instance()
    {
        static KclockFormat *singleton = new KclockFormat();
        return singleton;
    };

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
