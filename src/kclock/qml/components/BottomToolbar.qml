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
    position: ToolBar.Footer

    // set binding only after component has loaded, so we don't have an animation for the navigation bar coming in
    Component.onCompleted: shouldShow = Qt.binding(() => pageStack.layers.depth <= 1 && pageStack.depth <= 1);

    property bool shouldShow: true
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

    actions: [
        Kirigami.Action {
            icon.name: "clock"
            text: i18n("Time")
            checked: pageStack.currentItem?.objectName === "Time"
            onTriggered: {
                if (pageStack.currentItem?.objectName !== "Time") {
                    const page = applicationWindow().getPage("Time");
                    applicationWindow().switchToPage(page, 0);
                }
            }
        },
        Kirigami.Action {
            icon.name: "player-time"
            text: i18n("Timers")
            checked: pageStack.currentItem?.objectName === "Timers"
            onTriggered: {
                if (pageStack.currentItem?.objectName !== "Timers") {
                    const page = applicationWindow().getPage("Timers");
                    applicationWindow().switchToPage(page, 0);
                }
            }
        },
        Kirigami.Action {
            icon.name: "chronometer"
            text: i18n("Stopwatch")
            checked: pageStack.currentItem?.objectName === "Stopwatch"
            onTriggered: {
                if (pageStack.currentItem?.objectName !== "Stopwatch") {
                    const page = applicationWindow().getPage("Stopwatch");
                    applicationWindow().switchToPage(page, 0);
                }
            }
        },
        Kirigami.Action {
            icon.name: "notifications"
            text: i18n("Alarms")
            checked: pageStack.currentItem?.objectName === "Alarms"
            onTriggered: {
                if (pageStack.currentItem?.objectName !== "Alarms") {
                    const page = applicationWindow().getPage("Alarms");
                    applicationWindow().switchToPage(page, 0);
                }
            }
        }
    ]
}
