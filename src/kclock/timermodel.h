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

#ifndef KIRIGAMICLOCK_TIMERMODEL_H
#define KIRIGAMICLOCK_TIMERMODEL_H

#include <KLocalizedString>
#include <QAbstractListModel>
#include <QObject>

#include "timermodelinterface.h"
class Timer;
class TimerModel : public QAbstractListModel
{
    Q_OBJECT

public:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    static TimerModel *instance()
    {
        static TimerModel *singleton = new TimerModel();
        return singleton;
    }

    Q_INVOKABLE void addNew()
    {
        this->addTimer();
    };
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE int count();
    Q_INVOKABLE Timer *get(int index);

private Q_SLOTS:
    void addTimer(QString uuid); // remote add, always justCreated
    void removeTimer(QString uuid);

private:
    void addTimer(int length = 300, QString label = i18n("New timer"), bool running = false);
    void addTimer(QString uuid, bool justCreated);

    explicit TimerModel(QObject *parent = nullptr);
    QList<Timer *> m_timersList;
    OrgKdeKclockTimerModelInterface *m_interface;
};

#endif // KIRIGAMICLOCK_TIMERMODEL_H
