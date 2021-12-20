/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kclockformat.h"

#include "settingsmodel.h"
#include "utilmodel.h"

#include <QLocale>
#include <QTime>
#include <QTimer>

KclockFormat::KclockFormat(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
{
    connect(SettingsModel::instance(), &SettingsModel::timeFormatChanged, this, &KclockFormat::updateCurrentTime);
    connect(m_timer, &QTimer::timeout, this, &KclockFormat::updateCurrentTime);

    this->startTimer();
}

QString KclockFormat::currentTime()
{
    return m_currentTime;
}

void KclockFormat::updateCurrentTime()
{
    m_currentTime = QLocale::system().toString(QTime::currentTime(), UtilModel::instance()->timeFormat());
    Q_EMIT timeChanged();
}

QString KclockFormat::formatTimeString(int hours, int minutes)
{
    return QLocale::system().toString(QTime(hours, minutes), UtilModel::instance()->timeFormat());
}

bool KclockFormat::isChecked(int dayIndex, int daysOfWeek)
{
    // convert start day to standard start day of week, Monday
    dayIndex += QLocale::system().firstDayOfWeek() - 1;
    while (dayIndex > 6)
        dayIndex -= 7;

    int day = 1;
    day <<= dayIndex;
    return daysOfWeek & day;
}

void KclockFormat::startTimer()
{
    updateCurrentTime();
    m_timer->start(1000);
}

WeekModel::WeekModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // init week and flag value
    int dayFlag[] = {1, 2, 4, 8, 16, 32, 64}, i = 0;
    for (int j = QLocale::system().firstDayOfWeek(); j <= 7; j++) {
        m_listItem[i++] = std::tuple<QString, int>(QLocale::system().dayName(j, QLocale::LongFormat), dayFlag[j - 1]);
    }

    for (int j = 1; j < QLocale::system().firstDayOfWeek(); j++) {
        m_listItem[i++] = std::tuple<QString, int>(QLocale::system().dayName(j, QLocale::LongFormat), dayFlag[j - 1]);
    }
}

int WeekModel::rowCount(const QModelIndex &parent) const
{
    return 7;
}

QVariant WeekModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= 7) {
        return QVariant();
    }
    if (role == NameRole)
        return std::get<0>(m_listItem[index.row()]);
    else if (role == FlagRole)
        return std::get<1>(m_listItem[index.row()]);
    else
        return QVariant();
}

QHash<int, QByteArray> WeekModel::roleNames() const
{
    return {{NameRole, "name"}, {FlagRole, "flag"}};
}
