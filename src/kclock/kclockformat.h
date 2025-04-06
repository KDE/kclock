/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QQmlEngine>
#include <QTimer>

#include <array>
#include <tuple>

#include <qqmlintegration.h>

class QJSEngine;

class WeekModel;
class KclockFormat : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(KClockFormat)
    QML_SINGLETON
    Q_PROPERTY(QString currentTime READ currentTime NOTIFY timeChanged)

public:
    static KclockFormat *instance()
    {
        static KclockFormat *singleton = new KclockFormat();
        return singleton;
    };

    static KclockFormat *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
    {
        Q_UNUSED(qmlEngine);
        Q_UNUSED(jsEngine);
        auto *format = instance();
        QQmlEngine::setObjectOwnership(format, QQmlEngine::CppOwnership);
        return format;
    }

    QString currentTime() const;
    QString formatTimeString(int hours, int minutes);
    Q_INVOKABLE bool isChecked(int dayIndex, int daysOfWeek);

private Q_SLOTS:
    void updateCurrentTime();

Q_SIGNALS:
    void timeChanged();

private:
    explicit KclockFormat(QObject *parent = nullptr);

    void startTimer(); // basic settings for updating time display

    QTimer *m_timer;
    QString m_currentTime;
};

using weekListItem = std::array<std::tuple<QString, int>, 7>;

class WeekModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    enum {
        NameRole = Qt::DisplayRole,
        FlagRole = Qt::UserRole + 1
    };

    explicit WeekModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    weekListItem m_listItem;
};
