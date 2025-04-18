/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import org.kde.kclock as KClock

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
            checked: root.pageStack.currentItem?.objectName === "Time"
            onTriggered: {
                if (root.pageStack.currentItem?.objectName !== "Time") {
                    const page = applicationWindow().getPage("Time");
                    applicationWindow().switchToPage(page, 0);
                }
            }
        },
        Kirigami.Action {
            readonly property bool showTime: !checked && KClock.TimerModel.runningTimersCount > 0

            icon.name: "player-time"
            text: {
                if (showTime) {
                    const count = KClock.TimerModel.runningTimersCount;
                    if (count > 1) {
                        return i18np("%1 Timer", "%1 Timers", count);
                    } else if (KClock.TimerModel.runningTimer) {
                        return KClock.TimerModel.runningTimer.remainingPretty;
                    }
                }
                return i18n("Timers");
            }
            tooltip: showTime ? i18n("Timers") : ""
            checked: root.pageStack.currentItem?.objectName === "Timers"
            onTriggered: {
                if (root.pageStack.currentItem?.objectName !== "Timers") {
                    const page = applicationWindow().getPage("Timers");
                    applicationWindow().switchToPage(page, 0);
                }
            }
        },
        Kirigami.Action {
            readonly property bool showTime: !checked && !KClock.StopwatchTimer.stopped && !KClock.StopwatchTimer.paused

            icon.name: "chronometer"
            text: showTime ? KClock.StopwatchTimer.display : i18n("Stopwatch")
            tooltip: showTime ? i18n("Stopwatch") : ""
            checked: root.pageStack.currentItem?.objectName === "Stopwatch"
            onTriggered: {
                if (root.pageStack.currentItem?.objectName !== "Stopwatch") {
                    const page = applicationWindow().getPage("Stopwatch");
                    applicationWindow().switchToPage(page, 0);
                }
            }
        },
        Kirigami.Action {
            icon.name: "notifications"
            text: i18n("Alarms")
            checked: root.pageStack.currentItem?.objectName === "Alarms"
            onTriggered: {
                if (root.pageStack.currentItem?.objectName !== "Alarms") {
                    const page = applicationWindow().getPage("Alarms");
                    applicationWindow().switchToPage(page, 0);
                }
            }
        }
    ]
}
