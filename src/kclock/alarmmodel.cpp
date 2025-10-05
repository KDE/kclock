/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "alarmmodel.h"

#include "kclock_algorithm.hpp"
#include "utilmodel.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QQmlEngine>
#include <QXmlStreamReader>

AlarmModel *AlarmModel::instance()
{
    static AlarmModel *singleton = new AlarmModel();
    return singleton;
}

AlarmModel *AlarmModel::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine);
    Q_UNUSED(jsEngine);
    auto *model = instance();
    QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);
    return model;
}

AlarmModel::AlarmModel(QObject *parent)
    : QAbstractListModel{parent}
    , m_interface{new org::kde::kclock::AlarmModel(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Alarms"), QDBusConnection::sessionBus(), this)}
{
    connect(m_interface, &OrgKdeKclockAlarmModelInterface::alarmAdded, this, &AlarmModel::addAlarmInternal);
    connect(m_interface, &OrgKdeKclockAlarmModelInterface::alarmRemoved, this, &AlarmModel::removeAlarm);

    // watch for kclockd
    m_watcher = new QDBusServiceWatcher(QStringLiteral("org.kde.kclockd"), QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForOwnerChange, this);
    connect(m_watcher, &QDBusServiceWatcher::serviceRegistered, this, [this]() -> void {
        load();
    });
    connect(m_watcher, &QDBusServiceWatcher::serviceUnregistered, this, [this]() -> void {
        clear();
        setStatus(Status::NotConnected);
        setErrorString({});
    });

    load();
}

void AlarmModel::load()
{
    // load from dbus
    setStatus(Status::Loading);
    setErrorString({});

    // Unfortunately, Qt DBus doesn't support DBus ObjectManager.
    QDBusMessage message = QDBusMessage::createMethodCall(QStringLiteral("org.kde.kclockd"),
                                                          QStringLiteral("/Alarms"),
                                                          QStringLiteral("org.freedesktop.DBus.Introspectable"),
                                                          QStringLiteral("Introspect"));
    QDBusPendingCall call = QDBusConnection::sessionBus().asyncCall(message);
    auto *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher] {
        QDBusPendingReply<QString> reply = *watcher;

        if (reply.isError()) {
            qWarning() << "Failed to fetch alarms" << reply.error().name() << reply.error().message();
            clear();
            if (reply.error().type() == QDBusError::ServiceUnknown) {
                setStatus(Status::NotConnected);
            } else {
                setErrorString(reply.error().message());
                setStatus(Status::Error);
            }
        } else {
            beginResetModel();
            qDeleteAll(alarmsList);
            alarmsList.clear();

            QXmlStreamReader xml(reply.value());
            while (!xml.atEnd()) {
                xml.readNext();
                if (xml.isStartElement() && xml.name() == QLatin1String("node")) {
                    const auto name = xml.attributes().value(QLatin1String("name"));
                    if (!name.isEmpty() && !name.contains(QLatin1String("org"))) {
                        addAlarmInternal(name.toString());
                    }
                }
            }
            endResetModel();
            setStatus(Status::Ready);
        }

        watcher->deleteLater();
    });
}

AlarmModel::Status AlarmModel::status() const
{
    return m_status;
}

void AlarmModel::setStatus(AlarmModel::Status status)
{
    if (m_status != status) {
        m_status = status;
        Q_EMIT statusChanged(status);
    }
}

QString AlarmModel::errorString() const
{
    return m_errorString;
}

void AlarmModel::setErrorString(const QString &errorString)
{
    if (m_errorString != errorString) {
        m_errorString = errorString;
        Q_EMIT errorStringChanged(errorString);
    }
}

QHash<int, QByteArray> AlarmModel::roleNames() const
{
    return {{AlarmRole, "alarm"}};
}

QVariant AlarmModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= alarmsList.count() || role != AlarmRole) {
        return QVariant();
    }

    auto *alarm = alarmsList[index.row()];
    return alarm ? QVariant::fromValue(alarm) : QVariant();
}

int AlarmModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
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

void AlarmModel::addAlarm(const QString &name, int hours, int minutes, int daysOfWeek, const QString &audioPath, int ringDuration, int snoozeDuration)
{
    m_interface->addAlarm(name, hours, minutes, daysOfWeek, audioPath, ringDuration, snoozeDuration);
}

QString AlarmModel::timeToRingFormatted(int hours, int minutes, int daysOfWeek)
{
    return UtilModel::instance()->timeToRingFormatted(UtilModel::instance()->calculateNextRingTime(hours, minutes, daysOfWeek));
}

void AlarmModel::addAlarmInternal(QString uuid)
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

void AlarmModel::removeAlarm(const QString &uuid)
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

void AlarmModel::clear()
{
    beginResetModel();
    qDeleteAll(alarmsList);
    alarmsList.clear();
    endResetModel();
}

#include "moc_alarmmodel.cpp"
