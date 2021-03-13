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

#include <QLocale>
#include <QTime>
#include <QTimer>

#include "kclockformat.h"
#include "utilmodel.h"

KclockFormat::KclockFormat(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &KclockFormat::updateTime);
    this->startTimer();
}

void KclockFormat::updateTime()
{
    if (m_minutesCounter == 60) {
        m_currentTime = QLocale::system().toString(QTime::currentTime(), UtilModel::instance()->isLocale24HourTime() ? "hh:mm" : "h:mm ap");
        Q_EMIT timeChanged();
        m_minutesCounter = 0;
        m_hoursCounter++;
    }
    if (m_hoursCounter == 60) {
        m_hoursCounter = 0;
        m_hours = QTime::currentTime().hour() >= 12 ? QTime::currentTime().hour() - 12 : QTime::currentTime().hour();
        Q_EMIT hourChanged();
    }
    m_minutesCounter++;
    Q_EMIT secondChanged();
}

QString KclockFormat::formatTimeString(int hours, int minutes)
{
    return QLocale::system().toString(QTime(hours, minutes), QStringLiteral("hh:mm ap"));
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
    m_currentTime = QLocale::system().toString(QTime::currentTime(), UtilModel::instance()->isLocale24HourTime() ? "hh:mm" : "h:mm ap");
    m_hours = QTime::currentTime().hour() >= 12 ? QTime::currentTime().hour() - 12 : QTime::currentTime().hour();
    m_minutesCounter = (QTime::currentTime().msecsSinceStartOfDay() / 1000) % 60; // seconds to next minute
    m_hoursCounter = QTime::currentTime().minute();
    m_timer->start(1000);
}

WeekModel::WeekModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // init week and flag value
    int dayFlag[] = {1, 2, 4, 8, 16, 32, 64}, i = 0;
    for (int j = QLocale::system().firstDayOfWeek(); j <= 7; j++) {
        m_listItem[i++] = std::tuple<QString, int>(QLocale::system().dayName(j, QLocale::ShortFormat), dayFlag[j - 1]);
    }

    for (int j = 1; j < QLocale::system().firstDayOfWeek(); j++) {
        m_listItem[i++] = std::tuple<QString, int>(QLocale::system().dayName(j, QLocale::ShortFormat), dayFlag[j - 1]);
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
