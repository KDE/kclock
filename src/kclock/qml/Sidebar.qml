/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 *           2020 Devin Lin <espidev@gmail.com>
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

import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1 as QQC2
import org.kde.kirigami 2.11 as Kirigami

Kirigami.GlobalDrawer {
    id: drawer
    title: i18n("Clock")

    modal: false
    width: 200
    bannerVisible: false

    Kirigami.Theme.colorSet: Kirigami.Theme.Window
    Kirigami.Theme.inherit: false

    header: QQC2.ToolBar {
        implicitHeight: Kirigami.Units.gridUnit * 2 + Kirigami.Units.smallSpacing

        Item {
            anchors.fill: parent
            Kirigami.Heading {
                level: 1
                text: i18n("Clock")
                anchors.left: parent.left
                anchors.leftMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    actions: [
        Kirigami.Action {
            text: i18n("Time")
            iconName: "clock"
            onTriggered: {
                if (appwindow.pageStack.currentItem !== appwindow.getPage("Time")) {
                    appwindow.switchToPage(appwindow.getPage("Time"), 0);
                }
            }
        },
        Kirigami.Action {
            text: i18n("Timers")
            iconName: "player-time"
            onTriggered: {
                if (appwindow.pageStack.currentItem !== appwindow.getPage("Timers")) {
                    appwindow.switchToPage(appwindow.getPage("Timers"), 0);
                }
            }
        },
        Kirigami.Action {
            text: i18n("Stopwatch")
            iconName: "chronometer"
            onTriggered: {
                if (appwindow.pageStack.currentItem !== appwindow.getPage("Stopwatch")) {
                    appwindow.switchToPage(appwindow.getPage("Stopwatch"), 0);
                }
            }
        },
        Kirigami.Action {
            text: i18n("Alarms")
            iconName: "notifications"
            onTriggered: {
                if (appwindow.pageStack.currentItem !== appwindow.getPage("Alarms")) {
                    appwindow.switchToPage(appwindow.getPage("Alarms"), 0);
                }
            }
        },
        Kirigami.Action {
            text: i18n("Settings")
            icon.name: "settings-configure"
            onTriggered: {
                if (appwindow.pageStack.currentItem !== appwindow.getPage("Settings")) {
                    appwindow.switchToPage(appwindow.getPage("Settings"), 0);
                }
            }
        }
    ]
}
