/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
            checked: appwindow.pageStack.currentItem == appwindow.getPage("Time")
            onTriggered: {
                if (appwindow.pageStack.currentItem !== appwindow.getPage("Time")) {
                    appwindow.switchToPage(appwindow.getPage("Time"), 0);
                }
            }
        },
        Kirigami.Action {
            text: i18n("Timers")
            iconName: "player-time"
            checked: appwindow.pageStack.currentItem == appwindow.getPage("Timers")
            onTriggered: {
                if (appwindow.pageStack.currentItem !== appwindow.getPage("Timers")) {
                    appwindow.switchToPage(appwindow.getPage("Timers"), 0);
                }
            }
        },
        Kirigami.Action {
            text: i18n("Stopwatch")
            iconName: "chronometer"
            checked: appwindow.pageStack.currentItem == appwindow.getPage("Stopwatch")
            onTriggered: {
                if (appwindow.pageStack.currentItem !== appwindow.getPage("Stopwatch")) {
                    appwindow.switchToPage(appwindow.getPage("Stopwatch"), 0);
                }
            }
        },
        Kirigami.Action {
            text: i18n("Alarms")
            iconName: "notifications"
            checked: appwindow.pageStack.currentItem == appwindow.getPage("Alarms")
            onTriggered: {
                if (appwindow.pageStack.currentItem !== appwindow.getPage("Alarms")) {
                    appwindow.switchToPage(appwindow.getPage("Alarms"), 0);
                }
            }
        },
        Kirigami.Action {
            text: i18n("Settings")
            icon.name: "settings-configure"
            checked: appwindow.pageStack.currentItem == appwindow.getPage("Settings")
            onTriggered: {
                if (appwindow.pageStack.currentItem !== appwindow.getPage("Settings")) {
                    appwindow.switchToPage(appwindow.getPage("Settings"), 0);
                }
            }
        }
    ]
}
