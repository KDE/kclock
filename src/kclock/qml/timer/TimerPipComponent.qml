/*
 * Copyright 2025 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.kirigami as Kirigami

import org.kde.kclock as KClock

MouseArea {
    id: pipArea
    objectName: "TimerPip"

    // property alias doesn't work with required property timer on TimerComponent...
    property KClock.Timer timer

    onClicked: {
        pipArea.timer.toggleRunning()
    }

    KClock.TimerComponent {
        id: pipTimer
        timer: pipArea.timer
        anchors {
            fill: parent
            margins: Kirigami.Units.largeSpacing
        }

        maximizedCircle: true

        actions: [
            Kirigami.Action {
                text: i18nc("@action:button Add 1 minute to timer", "+1:00")
                icon.name: "list-add"
                displayHint: Kirigami.DisplayHint.IconOnly
                onTriggered: pipArea.timer.addMinute()
            },
            Kirigami.Action {
                text: pipArea.timer.running ? i18nc("@action:button", "Pause") : i18nc("@action:button", "Start")
                icon.name: pipArea.timer.running ? "chronometer-pause" : "chronometer-start"
                displayHint: Kirigami.DisplayHint.IconOnly
                onTriggered: pipArea.timer.toggleRunning()
            },
            Kirigami.Action {
                text: i18nc("@action:button", "Reset")
                icon.name: "chronometer-reset"
                displayHint: Kirigami.DisplayHint.IconOnly
                onTriggered: pipArea.timer.reset()
            }
        ]
        actionsVisible: KClock.PipHandler.hovered
        actionsInsideCircle: true
    }
}
