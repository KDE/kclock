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
    visible: applicationWindow().pageStack.layers.depth <= 1
    
    property var pageStack: applicationWindow().pageStack
    
    function getTimePage() {
        return applicationWindow().getPage("Time");
    }
    function getTimersPage() {
        return applicationWindow().getPage("Timers");
    }
    function getStopwatchPage() {
        return applicationWindow().getPage("Stopwatch");
    }
    function getAlarmsPage() {
        return applicationWindow().getPage("Alarms");
    }
    
    actions: [
        Kirigami.Action {
            iconName: "clock"
            text: i18n("Time")
            checked: getTimePage() === pageStack.currentItem
            onTriggered: {
                if (getTimePage()  !== pageStack.currentItem) {
                    applicationWindow().switchToPage(getTimePage(), 0);
                }
            }
        },
        Kirigami.Action {
            iconName: "player-time"
            text: i18n("Timers")
            checked: getTimersPage() === pageStack.currentItem
            onTriggered: {
                if (getTimersPage() !== pageStack.currentItem) {
                    applicationWindow().switchToPage(getTimersPage(), 0);
                }
            }
        },
        Kirigami.Action {
            iconName: "chronometer"
            text: i18n("Stopwatch")
            checked: getStopwatchPage() === pageStack.currentItem
            onTriggered: {
                if (getStopwatchPage() !== pageStack.currentItem) {
                    applicationWindow().switchToPage(getStopwatchPage(), 0);
                }
            }
        },
        Kirigami.Action {
            iconName: "notifications"
            text: i18n("Alarms")
            checked: getAlarmsPage() === pageStack.currentItem
            onTriggered: {
                if (getAlarmsPage() !== pageStack.currentItem) {
                    applicationWindow().switchToPage(getAlarmsPage(), 0);
                }
            }
        }
    ]
} 
