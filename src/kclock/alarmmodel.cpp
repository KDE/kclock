/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "alarmmodel.h"

#include "alarm.h"
#include "kclock_algorithm.hpp"
#include "utilmodel.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <KStatusNotifierItem>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QLocale>
#include <QThread>
#include <QXmlStreamReader>

AlarmModel *AlarmModel::instance()
{
    static AlarmModel *singleton = new AlarmModel();
    return singleton;
}

AlarmModel::AlarmModel(QObject *parent)
    : QAbstractListModel{parent}
    , m_interface{new org::kde::kclock::AlarmModel(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Alarms"), QDBusConnection::sessionBus(), this)}
{
    if (m_interface->isValid()) {
        connect(m_interface, SIGNAL(alarmAdded(QString)), this, SLOT(addAlarm(QString)));
        connect(m_interface, SIGNAL(alarmRemoved(QString)), this, SLOT(removeAlarm(QString)));
    }
    setConnectedToDaemon(m_interface->isValid());
    load();

    // watch for kclockd
    m_watcher = new QDBusServiceWatcher(QStringLiteral("org.kde.kclockd"), QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForOwnerChange, this);
    connect(m_watcher, &QDBusServiceWatcher::serviceRegistered, this, [this]() -> void {
        setConnectedToDaemon(true);
        if (m_interface->isValid()) {
            connect(m_interface, SIGNAL(alarmAdded(QString)), this, SLOT(addAlarm(QString)));
            connect(m_interface, SIGNAL(alarmRemoved(QString)), this, SLOT(removeAlarm(QString)));
        }
    });
    connect(m_watcher, &QDBusServiceWatcher::serviceUnregistered, this, [this]() -> void {
        setConnectedToDaemon(false);
    });
}

void AlarmModel::load()
{
    // load from dbus
    QDBusInterface *interface = new QDBusInterface(QStringLiteral("org.kde.kclockd"),
                                                   QStringLiteral("/Alarms"),
                                                   QStringLiteral("org.freedesktop.DBus.Introspectable"),
                                                   QDBusConnection::sessionBus(),
                                                   this);
    QDBusReply<QString> reply = interface->call(QStringLiteral("Introspect"));
    if (reply.isValid()) {
        QXmlStreamReader xml(reply.value());
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.name() == QStringLiteral("node") && xml.attributes().hasAttribute(QStringLiteral("name"))) {
                if (xml.attributes().value(QStringLiteral("name")).toString().indexOf(QStringLiteral("org")) == -1) {
                    this->addAlarm(xml.attributes().value(QStringLiteral("name")).toString());
                }
            }
        }
    }

    interface->deleteLater();
}

QHash<int, QByteArray> AlarmModel::roleNames() const
{
    return {{AlarmRole, "alarm"}};
}

QVariant AlarmModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= alarmsList.count()) {
        return QVariant();
    }

    auto *alarm = alarmsList[index.row()];
    return alarm ? QVariant::fromValue(alarm) : QVariant();
}

int AlarmModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return alarmsList.size();
}

Qt::ItemFlags AlarmModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEditable;
}

void AlarmModel::remove(int index)
{
    if (index < 0 || index >= this->alarmsList.size())
        return;

    m_interface->removeAlarm(alarmsList.at(index)->uuid().toString());
    auto ptr = alarmsList.at(index);

    Q_EMIT beginRemoveRows(QModelIndex(), index, index);
    alarmsList.removeAt(index);
    Q_EMIT endRemoveRows();

    ptr->deleteLater();
}

void AlarmModel::addAlarm(QString name, int hours, int minutes, int daysOfWeek, QString audioPath, int ringDuration, int snoozeDuration)
{
    m_interface->addAlarm(name, hours, minutes, daysOfWeek, audioPath, ringDuration, snoozeDuration);
}

QString AlarmModel::timeToRingFormatted(int hours, int minutes, int daysOfWeek)
{
    return UtilModel::instance()->timeToRingFormatted(UtilModel::instance()->calculateNextRingTime(hours, minutes, daysOfWeek));
}

void AlarmModel::addAlarm(QString uuid)
{
    auto alarm = new Alarm(uuid.remove(QRegularExpression(QStringLiteral("[{}-]"))));
    auto index = KClock::insert_index(alarm, alarmsList, [](Alarm *const &left, Alarm *const &right) {
        if (left->hours() < right->hours()) {
            return true;
        } else if (left->hours() > right->hours()) {
            return false;
        } else if (left->minutes() <= right->minutes()) {
            return true;
        } else {
            return false;
        }
    });

    Q_EMIT beginInsertRows(QModelIndex(), index, index);
    alarmsList.insert(index, alarm);
    Q_EMIT endInsertRows();
}

void AlarmModel::removeAlarm(QString uuid)
{
    auto index = 0;
    for (auto alarm : alarmsList) {
        if (alarm->uuid().toString() == uuid) {
            break;
        }
        ++index;
    }

    if (index >= this->alarmsList.size())
        return;

    auto ptr = alarmsList.at(index);

    Q_EMIT beginRemoveRows(QModelIndex(), index, index);
    alarmsList.removeAt(index);
    Q_EMIT endRemoveRows();

    ptr->deleteLater();
}

bool AlarmModel::connectedToDaemon()
{
    return m_connectedToDaemon;
}

void AlarmModel::setConnectedToDaemon(bool connectedToDaemon)
{
    if (m_connectedToDaemon != connectedToDaemon) {
        m_connectedToDaemon = connectedToDaemon;
        Q_EMIT connectedToDaemonChanged();
    }
}
