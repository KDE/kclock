// SPDX-FileCopyrightText: 2024 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QAbstractListModel>

#include <qqmlintegration.h>

class QJSEngine;
class QQmlEngine;

// model for stopwatch laps
class StopwatchModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(qint64 mostRecentLapTime READ mostRecentLapTime NOTIFY mostRecentLapTimeChanged)

public:
    static StopwatchModel *instance();
    static StopwatchModel *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    struct StopwatchLap {
        int lapNumber;
        qreal lapTime;
        qreal lapTimeSinceBeginning;
    };

    enum Roles {
        LapNumber = Qt::DisplayRole,
        LapTime,
        LapTimeSinceBeginning,
        IsBest,
        IsWorst,
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addLap();

    qint64 mostRecentLapTime();

Q_SIGNALS:
    void mostRecentLapTimeChanged();

private Q_SLOTS:
    void reset();

private:
    explicit StopwatchModel(QObject *parent = nullptr);

    QList<StopwatchLap> m_laps; // sorted from newest to oldest
    qint64 m_mostRecentLapTime = 0;

    qint64 m_worstLapTime = 0;
    int m_worstLapIndex = -1; // flipped

    qint64 m_bestLapTime = 0;
    int m_bestLapIndex = -1; // flipped
};
