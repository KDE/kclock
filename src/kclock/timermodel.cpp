/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
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
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtGlobal>

#include <KLocalizedString>

#include "timer.h"
#include "timermodel.h"

/* ~ TimerModel ~ */
TimerModel::TimerModel(QObject *parent)
    : m_interface(new OrgKdeKclockTimerModelInterface(QStringLiteral("org.kde.kclockd"), QStringLiteral("/Timers"), QDBusConnection::sessionBus(), this))
{
    if (m_interface->isValid()) {
        connect(m_interface, SIGNAL(timerAdded(QString)), this, SLOT(addTimer(QString)));
        connect(m_interface, SIGNAL(timerRemoved(QString)), this, SLOT(removeTimer(QString)));
    }
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

void TimerModel::addTimer(int length, QString label, bool running)
{
    m_interface->addTimer(length, label, running);
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
