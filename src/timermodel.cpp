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

#include "timermodel.h"

#include <KNotification>
#include <KLocalizedString>

#include <QCoreApplication>
#include <QDebug>
#include <QtGlobal>
#include <QApplication>

TimerModel::TimerModel(QObject* parent) {}

void TimerModel::timerFinished()
{
    qDebug("Timer finished, sending notification...");
    KNotification* notification = new KNotification("timerFinished");
    
    notification->setIconName("kronometer");
    notification->setTitle(i18n("Timer complete"));
    notification->setText(i18n("Your timer has finished!"));
    notification->setDefaultAction(i18n("View"));
    notification->setUrgency(KNotification::HighUrgency);
    notification->setFlags(KNotification::NotificationFlag::RaiseWidgetOnActivation | 
                            KNotification::NotificationFlag::LoopSound | 
                            KNotification::NotificationFlag::SkipGrouping |
                            KNotification::NotificationFlag::CloseWhenWidgetActivated);

    notification->sendEvent();
}
