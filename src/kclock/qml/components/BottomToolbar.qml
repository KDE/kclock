/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.NavigationTabBar {
    id: root

    property bool shouldShow: pageStack.layers.depth <= 1 && pageStack.depth <= 1
    onShouldShowChanged: {
        if (shouldShow) {
            hideAnim.stop();
            showAnim.restart();
        } else {
            showAnim.stop();
            hideAnim.restart();
        }
    }

    visible: height !== 0

    // animate showing and hiding of navbar
    ParallelAnimation {
        id: showAnim
        NumberAnimation {
            target: root
            property: "height"
            to: root.implicitHeight
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: root
            property: "opacity"
            to: 1
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    SequentialAnimation {
        id: hideAnim
        NumberAnimation {
            target: root
            property: "opacity"
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: root
            property: "height"
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

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
            icon.name: "clock"
            text: i18n("Time")
            checked: getTimePage() === pageStack.currentItem
            onTriggered: {
                if (getTimePage()  !== pageStack.currentItem) {
                    applicationWindow().switchToPage(getTimePage(), 0);
                }
            }
        },
        Kirigami.Action {
            icon.name: "player-time"
            text: i18n("Timers")
            checked: getTimersPage() === pageStack.currentItem
            onTriggered: {
                if (getTimersPage() !== pageStack.currentItem) {
                    applicationWindow().switchToPage(getTimersPage(), 0);
                }
            }
        },
        Kirigami.Action {
            icon.name: "chronometer"
            text: i18n("Stopwatch")
            checked: getStopwatchPage() === pageStack.currentItem
            onTriggered: {
                if (getStopwatchPage() !== pageStack.currentItem) {
                    applicationWindow().switchToPage(getStopwatchPage(), 0);
                }
            }
        },
        Kirigami.Action {
            icon.name: "notifications"
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
