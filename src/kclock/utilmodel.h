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

#ifndef KIRIGAMICLOCK_UTILMODEL_H
#define KIRIGAMICLOCK_UTILMODEL_H

#include <QObject>
#include <QString>

class UtilModel;
class UtilModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString tzName READ getCurrentTimeZoneName NOTIFY propertyChanged)

public:
    static UtilModel *instance()
    {
        static UtilModel * singleton = new UtilModel;
        return singleton;
    }
    
    QString getCurrentTimeZoneName();
    bool applicationLoaded();
    void setApplicationLoaded(bool applicationLoaded);
    
    long long calculateNextRingTime(int hours, int minutes, int daysOfWeek, int snooze = 0);
    QString timeToRingFormatted(const long long &timestamp);
    
private:
    UtilModel(){}; // explicitly declare it private
    bool m_applicationLoaded = false;
    
Q_SIGNALS:
    void propertyChanged();
    void applicationLoadedChanged();
};

#endif // KIRIGAMICLOCK_UTILMODEL_H
