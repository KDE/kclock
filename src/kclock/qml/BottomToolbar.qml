/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.19 as Kirigami
import QtGraphicalEffects 1.12

Kirigami.NavigationTabBar {
    id: root
    visible: appwindow.pageStack.layers.depth <= 1
    
    actions: [
        Kirigami.Action {
            iconName: "clock"
            text: i18n("Time")
            checked: appwindow.getPage("Time") === appwindow.pageStack.currentItem
            onTriggered: {
                if (appwindow.getPage("Time") !== appwindow.pageStack.currentItem) {
                    appwindow.switchToPage(appwindow.getPage("Time"), 0);
                }
            }
        },
        Kirigami.Action {
            iconName: "player-time"
            text: i18n("Timers")
            checked: appwindow.getPage("Timers") === appwindow.pageStack.currentItem
            onTriggered: {
                if (appwindow.getPage("Timers") !== appwindow.pageStack.currentItem) {
                    appwindow.switchToPage(appwindow.getPage("Timers"), 0);
                }
            }
        },
        Kirigami.Action {
            iconName: "chronometer"
            text: i18n("Stopwatch")
            checked: appwindow.getPage("Stopwatch") === appwindow.pageStack.currentItem
            onTriggered: {
                if (appwindow.getPage("Stopwatch") !== appwindow.pageStack.currentItem) {
                    appwindow.switchToPage(appwindow.getPage("Stopwatch"), 0);
                }
            }
        },
        Kirigami.Action {
            iconName: "notifications"
            text: i18n("Alarms")
            checked: appwindow.getPage("Alarms") === appwindow.pageStack.currentItem
            onTriggered: {
                if (appwindow.getPage("Alarms") !== appwindow.pageStack.currentItem) {
                    appwindow.switchToPage(appwindow.getPage("Alarms"), 0);
                }
            }
        },
        Kirigami.Action {
            iconName: "settings-configure"
            text: i18n("Settings")
            checked: appwindow.getPage("Settings") === appwindow.pageStack.currentItem
            onTriggered: {
                if (appwindow.getPage("Settings") !== appwindow.pageStack.currentItem) {
                    appwindow.switchToPage(appwindow.getPage("Settings"), 0);
                }
            }
        }
    ]
} 
