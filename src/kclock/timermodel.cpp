/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "timermodel.h"

#include "timer.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtGlobal>

#include <KLocalizedString>

/* ~ TimerModel ~ */
TimerModel::TimerModel(QObject *parent)
    : m_interface(new OrgKdeKclockTimerModelInterface(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Timers"), QDBusConnection::sessionBus(), this))
{
    if (m_interface->isValid()) {
        connect(m_interface, SIGNAL(timerAdded(QString)), this, SLOT(addTimer(QString)));
        connect(m_interface, SIGNAL(timerRemoved(QString)), this, SLOT(removeTimer(QString)));
    }
    setConnectedToDaemon(m_interface->isValid());

    QDBusInterface *interface = new QDBusInterface(QStringLiteral("org.kde.kclockd"),
                                                   QStringLiteral("/Timers"),
                                                   QStringLiteral("org.freedesktop.DBus.Introspectable"),
                                                   QDBusConnection::sessionBus(),
                                                   this);
    QDBusReply<QString> reply = interface->call(QStringLiteral("Introspect"));
    if (reply.isValid()) {
        auto xmlMsg = reply.value();
        QXmlStreamReader xml(xmlMsg);
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.name() == QStringLiteral("node") && xml.attributes().hasAttribute(QStringLiteral("name"))) {
                if (xml.attributes().value(QStringLiteral("name")).toString().indexOf(QStringLiteral("org")) == -1) {
                    // already existed on kclock launch, not justCreated
                    this->addTimer(xml.attributes().value(QStringLiteral("name")).toString(), false);
                }
            }
        }
    }
    interface->deleteLater();

    // watch for kclockd
    m_watcher = new QDBusServiceWatcher(QStringLiteral("org.kde.kclockd"), QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForOwnerChange, this);
    connect(m_watcher, &QDBusServiceWatcher::serviceRegistered, this, [this]() -> void {
        setConnectedToDaemon(true);
    });
    connect(m_watcher, &QDBusServiceWatcher::serviceUnregistered, this, [this]() -> void {
        setConnectedToDaemon(false);
    });
}

int TimerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_timersList.size();
}

int TimerModel::count()
{
    return m_timersList.size();
}

QVariant TimerModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

void TimerModel::addTimer(int length, QString label, QString commandTimeout, bool running)
{
    m_interface->addTimer(length, label, commandTimeout, running);
}

void TimerModel::remove(int index)
{
    if (index < 0 || index >= m_timersList.size())
        return;

    m_interface->removeTimer(m_timersList.at(index)->uuid().toString());
    m_timersList.at(index)->deleteLater();

    Q_EMIT beginRemoveRows(QModelIndex(), index, index);
    m_timersList.removeAt(index);
    Q_EMIT endRemoveRows();
}

Timer *TimerModel::get(int index)
{
    if ((index < 0) || (index >= m_timersList.count()))
        return {};

    return m_timersList.at(index);
}

void TimerModel::addTimer(QString uuid)
{
    this->addTimer(uuid, true);
}

void TimerModel::addTimer(QString uuid, bool justCreated)
{
    auto *timer = new Timer(uuid.remove(QRegularExpression(QStringLiteral("[{}-]"))), justCreated);

    Q_EMIT beginInsertRows(QModelIndex(), 0, 0);
    m_timersList.insert(0, timer);
    Q_EMIT endInsertRows();
}

void TimerModel::removeTimer(QString uuid)
{
    auto index = 0;
    for (auto timer : m_timersList) {
        if (timer->uuid().toString() == uuid) {
            break;
        }
        ++index;
    }

    // don't need to check index out of bound, remove(index) takes care of that
    remove(index);
}

bool TimerModel::connectedToDaemon()
{
    return m_connectedToDaemon;
}

void TimerModel::setConnectedToDaemon(bool connectedToDaemon)
{
    if (m_connectedToDaemon != connectedToDaemon) {
        m_connectedToDaemon = connectedToDaemon;
        Q_EMIT connectedToDaemonChanged();
    }
}
