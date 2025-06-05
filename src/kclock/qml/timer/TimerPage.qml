/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.kclock

Kirigami.Page {
    id: root

    objectName: "Timer"

    required property Timer timer

    title: timer && timer.label !== "" ? timer.label : i18n("New timer")
    readonly property string hiddenTitle: running ? timer.remainingPretty : ""
    background: null

    property bool showFullscreen: false
    readonly property bool minimizedToPip: (PipHandler.currentItem?.objectName === "TimerPip" && PipHandler.currentItem?.timer === root.timer) ?? false

    property bool running: timer ? timer.running : 0
    property bool looping: timer ? timer.looping : 0
    property string commandTimeout: timer ? timer.commandTimeout : ""
    property bool isCommandTimeout: commandTimeout.length > 0

    // keyboard controls
    Keys.onSpacePressed: timer.toggleRunning();
    Keys.onReturnPressed: timer.toggleRunning();

    // topbar action
    actions: [
        Kirigami.Action {
            text: root.running ? i18n("Pause") : i18n("Start")
            icon.name: root.running ? "chronometer-pause" : "chronometer-start"
            displayHint: Kirigami.DisplayHint.KeepVisible
            visible: !Kirigami.Settings.isMobile
            onTriggered: root.timer.toggleRunning()
        },
        Kirigami.Action {
            icon.name: "chronometer-reset"
            text: i18n("Reset")
            visible: !Kirigami.Settings.isMobile
            onTriggered: root.timer.reset();
        },
        Kirigami.Action {
            icon.name: "delete"
            text: i18n("Delete")
            visible: !Kirigami.Settings.isMobile
            onTriggered: {
                applicationWindow().pageStack.pop();
                TimerModel.remove(root.timer);
            }
        },
        Kirigami.Action {
            icon.name: "media-repeat-all"
            text: i18n("Loop Timer")
            checkable: true
            checked: looping
            visible: !Kirigami.Settings.isMobile
            onTriggered: root.timer.toggleLooping()
        },
        Kirigami.Action {
            id: pipAction
            icon.name: root.minimizedToPip ? "window-restore-pip" : "window-minimize-pip"
            text: root.minimizedToPip ? i18nc("@action", "Restore") : i18nc("@action", "Pop out")
            displayHint: Kirigami.DisplayHint.IconOnly | Kirigami.DisplayHint.KeepVisible
            visible: PipHandler.supported
            onTriggered: {
                if (root.minimizedToPip) {
                    PipHandler.hide();
                } else {
                    PipHandler.show(timerPipComponent, {timer: root.timer});
                }
            }
        }
    ]

    // mobile footer actions
    footer: ToolBar {
        id: toolbar
        visible: Kirigami.Settings.isMobile
        height: Kirigami.Settings.isMobile ? implicitHeight : 0
        topPadding: 0; bottomPadding: 0
        rightPadding: 0; leftPadding: 0

        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false

        property bool opened: false
        RowLayout {
            anchors.fill: parent
            spacing: 0

            Item { Layout.fillWidth: true }
            FooterToolBarButton {
                display: toolbar.opened ? AbstractButton.TextUnderIcon : AbstractButton.TextOnly
                text: root.running ? i18n("Pause") : i18n("Start")
                icon.name: root.running ? "chronometer-pause" : "chronometer-start"
                onClicked: root.timer.toggleRunning()
            }
            FooterToolBarButton {
                display: toolbar.opened ? AbstractButton.TextUnderIcon : AbstractButton.TextOnly
                text: i18n("Reset")
                icon.name: "chronometer-reset"
                onClicked: root.timer.reset()
            }
            FooterToolBarButton {
                display: toolbar.opened ? AbstractButton.TextUnderIcon : AbstractButton.TextOnly
                text: i18n("Delete")
                icon.name: "delete"
                onClicked: {
                    applicationWindow().pageStack.pop();
                    TimerModel.remove(root.timer);
                }
            }
            FooterToolBarButton {
                display: toolbar.opened ? AbstractButton.TextUnderIcon : AbstractButton.TextOnly
                text: i18n("Loop Timer")
                icon.name: "media-repeat-all"
                checked: root.looping
                onClicked: root.timer.toggleLooping()
            }
            FooterToolBarButton {
                display: toolbar.opened ? AbstractButton.TextUnderIcon : AbstractButton.TextOnly
                icon.name: "view-more-symbolic"
                onClicked: toolbar.opened = !toolbar.opened
                iconSize: Kirigami.Units.iconSizes.small
                implicitWidth: Kirigami.Units.gridUnit * 2.5
            }
        }
    }

    Component {
        id: timerPipComponent

        MouseArea {
            id: pipArea
            objectName: "TimerPip"

            // property alias doesn't work with required property timer on TimerComponent...
            required property Timer timer

            onClicked: root.timer.toggleRunning()

            TimerComponent {
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
                        onTriggered: root.timer.addMinute()
                    },
                    Kirigami.Action {
                        text: root.running ? i18nc("@action:button", "Pause") : i18nc("@action:button", "Start")
                        icon.name: root.running ? "chronometer-pause" : "chronometer-start"
                        displayHint: Kirigami.DisplayHint.IconOnly
                        onTriggered: root.timer.toggleRunning()
                    },
                    Kirigami.Action {
                        text: i18nc("@action:button", "Reset")
                        icon.name: "chronometer-reset"
                        displayHint: Kirigami.DisplayHint.IconOnly
                        onTriggered: root.timer.reset()
                    }
                ]
                actionsVisible: PipHandler.hovered
            }
        }
    }

    TimerComponent {
        anchors.fill: parent
        timer: root.timer
        visible: !root.minimizedToPip

        actions: [
            Kirigami.Action {
                text: i18nc("@action:button Add 1 minute to timer", "1 minute")
                icon.name: "list-add"
                onTriggered: root.timer.addMinute()
            }
        ]
    }

    Kirigami.PlaceholderMessage {
        width: parent.width
        anchors.verticalCenter: parent.verticalCenter
        text: i18n("This timer is in Picture-in-Picture mode.")
        icon.name: "window-minimize-pip"
        visible: root.minimizedToPip
        helpfulAction: Kirigami.Action {
            icon.name: "window-restore-pip"
            text: i18nc("@action:button Restore (unminimize) from pip mode", "Restore")
            onTriggered: PipHandler.hide()
        }
    }

    RowLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        Kirigami.Icon {
            source: "dialog-scripts"
            visible: root.isCommandTimeout
            implicitWidth: Kirigami.Units.iconSizes.sizeForLabels * 1.5
            implicitHeight: Kirigami.Units.iconSizes.sizeForLabels * 1.5
            color: Kirigami.Theme.disabledTextColor
        }
        Label {
            visible: root.isCommandTimeout
            font.family: "Monospace"
            text: root.commandTimeout
            color: Kirigami.Theme.disabledTextColor
        }
    }
}
