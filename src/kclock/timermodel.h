/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <KLocalizedString>

#include <QAbstractListModel>
#include <QObject>

#include <qqmlintegration.h>

#include "timermodelinterface.h"

class QQmlEngine;
class QJSEngine;

class Timer;
class TimerModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(bool connectedToDaemon READ connectedToDaemon NOTIFY connectedToDaemonChanged)
    Q_PROPERTY(QString defaultAudioLocation READ defaultAudioLocation WRITE setDefaultAudioLocation NOTIFY defaultAudioLocationChanged)
    /**
     * If there is a single running timer in the model, this property will return it.
     */
    Q_PROPERTY(Timer *runningTimer READ runningTimer NOTIFY runningTimerChanged)

public:
    static TimerModel *instance();
    static TimerModel *create(QQmlEngine *qmlengine, QJSEngine *jsEngine);

    enum {
        TimerRole,
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addNew(int length, const QString &label, bool looping, const QString &commandTimeout);
    Q_INVOKABLE void remove(Timer *timer);

    bool connectedToDaemon();
    void setConnectedToDaemon(bool connectedToDaemon);

    QString defaultAudioLocation() const;
    void setDefaultAudioLocation(const QString &location);

    Timer *runningTimer() const;

Q_SIGNALS:
    void connectedToDaemonChanged();
    void runningTimerChanged();
    void defaultAudioLocationChanged();

private Q_SLOTS:
    void addTimer(QString uuid);
    void removeTimer(const QString &uuid);
    void updateDefaultAudioLocation();

private:
    explicit TimerModel(QObject *parent = nullptr);

    QList<Timer *> m_timersList;
    OrgKdeKclockTimerModelInterface *const m_interface;
    QDBusServiceWatcher *m_watcher;
    QString m_defaultAudioLocation;
    bool m_connectedToDaemon = false;
};
