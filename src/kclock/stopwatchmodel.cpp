// SPDX-FileCopyrightText: 2024 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "stopwatchmodel.h"
#include "stopwatchtimer.h"

#include <QQmlEngine>

StopwatchModel *StopwatchModel::instance()
{
    static StopwatchModel *instance = new StopwatchModel;
    return instance;
}

StopwatchModel *StopwatchModel::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine);
    Q_UNUSED(jsEngine);
    auto *model = instance();
    QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);
    return model;
}

StopwatchModel::StopwatchModel(QObject *parent)
    : QAbstractListModel{parent}
{
    // clear model when stopwatch is reset
    connect(StopwatchTimer::instance(), &StopwatchTimer::resetTriggered, this, &StopwatchModel::reset);
}

int StopwatchModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_laps.size();
}

QVariant StopwatchModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();

    if (row < 0 || row >= m_laps.size()) {
        return QVariant{};
    }

    const auto lap = m_laps[row];

    switch (role) {
    case LapNumber:
        return lap.lapNumber;
    case LapTime:
        return lap.lapTime;
    case LapTimeSinceBeginning:
        return lap.lapTimeSinceBeginning;
    case IsBest:
        return lap.lapNumber == m_bestLapIndex;
    case IsWorst:
        return lap.lapNumber == m_worstLapIndex;
    }

    return {};
}

QHash<int, QByteArray> StopwatchModel::roleNames() const
{
    return {{LapNumber, "lapNumber"}, {LapTime, "lapTime"}, {LapTimeSinceBeginning, "lapTimeSinceBeginning"}, {IsBest, "isBest"}, {IsWorst, "isWorst"}};
}

void StopwatchModel::addLap()
{
    beginInsertRows(QModelIndex(), 0, 0);

    // time since stopwatch started
    const qint64 elapsedSinceBeginning = StopwatchTimer::instance()->elapsedTime();

    StopwatchLap lap{};
    lap.lapNumber = m_laps.size() + 1; // start at 1

    // time since last lap
    lap.lapTime = (m_laps.size() > 0) ? (elapsedSinceBeginning - m_laps[0].lapTimeSinceBeginning) : elapsedSinceBeginning;

    // time since stopwatch started
    lap.lapTimeSinceBeginning = elapsedSinceBeginning;

    // insert at front
    m_laps.insert(0, lap);

    // update most recent lap time
    m_mostRecentLapTime = elapsedSinceBeginning;
    Q_EMIT mostRecentLapTimeChanged();

    QList<int> toUpdate;

    // update worst lap time
    if (m_worstLapTime <= lap.lapTime || m_worstLapIndex == -1) {
        int oldIndex = m_worstLapIndex;
        m_worstLapIndex = lap.lapNumber;
        m_worstLapTime = lap.lapTime;

        if (oldIndex != -1) { // ensure old row is updated
            toUpdate.push_back(m_laps.size() - oldIndex);
        }
    }

    // update best lap time
    if (m_bestLapTime >= lap.lapTime || m_bestLapIndex == -1) {
        int oldIndex = m_bestLapIndex;
        m_bestLapIndex = lap.lapNumber;
        m_bestLapTime = lap.lapTime;

        if (oldIndex != -1) { // ensure old row is updated
            toUpdate.push_back(m_laps.size() - oldIndex);
        }
    }

    endInsertRows();

    // only emit signal after endInsertRows()
    for (int update : toUpdate) {
        QModelIndex index = createIndex(update, 0);
        Q_EMIT dataChanged(index, index, {IsBest, IsWorst});
    }
}

qint64 StopwatchModel::mostRecentLapTime()
{
    return m_mostRecentLapTime;
}

void StopwatchModel::reset()
{
    // clear model
    beginResetModel();
    m_laps.clear();
    endResetModel();

    // reset fields
    m_mostRecentLapTime = 0;
    Q_EMIT mostRecentLapTimeChanged();
    m_worstLapIndex = -1;
    m_bestLapIndex = -1;
}

#include "moc_stopwatchmodel.cpp"
