/*
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.kclock

Kirigami.AbstractCard {
    id: root

    signal editClicked()

    property Timer timer
    
    readonly property int length: timer ? timer.length : 1
    readonly property int elapsed: timer ? timer.elapsed : 0
    readonly property string lengthPretty: timer ? timer.lengthPretty : ""
    readonly property string remainingPretty: timer ? timer.remainingPretty : ""
    readonly property string label: timer ? timer.label : ""
    readonly property string commandTimeout: timer ? timer.commandTimeout : ""
    readonly property bool running: timer ? timer.running : false
    readonly property bool looping: timer ? timer.looping : false
    
    showClickFeedback: true

    // timer ringing popup
    Loader {
        id: popupLoader
        active: false
        sourceComponent: TimerRingingPopup {
            timer: root.timer
            onVisibleChanged: {
                if (!visible) {
                    popupLoader.active = false;
                }
            }
        }
        
        Component.onCompleted: determineState()
        function determineState() : void {
            if (root.timer.ringing) {
                popupLoader.active = true;
                popupLoader.item.open();
            } else if (popupLoader.item) {
                popupLoader.item.close();
            }
        }
        
        Connections {
            target: root.timer
            ignoreUnknownSignals: true
            
            function onRingingChanged() {
                popupLoader.determineState();
            }
        }
    }
    
    // timer card contents
    contentItem: ColumnLayout {
        id: delegateLayout

        spacing: Kirigami.Units.largeSpacing

        RowLayout {
            Layout.fillWidth: true

            Kirigami.Heading {
                id: label
                level: 2
                Layout.fillWidth: true
                text: root.label || i18n("New timer")
                elide: Text.ElideRight
                textFormat: Text.PlainText

                HoverHandler {
                    id: labelHoverHandler
                }
                TapHandler {
                    id: labelTapHandler
                }

                ToolTip.visible: (Kirigami.Settings.tabletMode ? labelTapHandler.pressed : labelHoverHandler.hovered) && ToolTip.text !== ""
                ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                ToolTip.text: label.truncated ? label.text : ""
            }

            Kirigami.Icon {
                Layout.preferredWidth: Kirigami.Units.iconSizes.small
                Layout.preferredHeight: Kirigami.Units.iconSizes.small
                source: "media-repeat-all"
                visible: root.timer.looping
                Accessible.name: i18n("Loop Timer")

                HoverHandler {
                    id: loopingHoverHandler
                }
                TapHandler {
                    id: loopingTapHandler
                }

                ToolTip.visible: (Kirigami.Settings.tabletMode ? loopingTapHandler.pressed : loopingHoverHandler.hovered)
                ToolTip.delay: Kirigami.Units.toolTipDelay
                ToolTip.text: Accessible.name
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 0

            ProgressBar {
                property real progress: root.elapsed / root.length

                Layout.fillWidth: true

                value: 0
                Component.onCompleted: value = progress

                onProgressChanged: {
                    progressTransition.to = progress;
                    progressTransition.restart();
                }
                NumberAnimation on value {
                    id: progressTransition
                    duration: 300
                    easing.type: Easing.InOutQuad
                }
            }

            RowLayout {
                Layout.fillWidth: true

                Label {
                    text: root.remainingPretty
                    color: root.running ? Kirigami.Theme.activeTextColor : Kirigami.Theme.disabledTextColor
                }
                Item {
                    Layout.fillWidth: true
                }
                Label {
                    text: root.lengthPretty
                    color: root.running ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                }
            }
        }

        RowLayout {
            id: buttonLayout

            Layout.fillWidth: true

            spacing: Kirigami.Units.smallSpacing

            Button {
                icon.name: root.running ? "chronometer-pause" : "chronometer-start"
                display: AbstractButton.TextBesideIcon
                text: root.running ? i18nc("@info:tooltip", "Pause") : i18nc("@info:tooltip", "Start")
                onClicked: root.timer.toggleRunning()
            }
            Item {
                Layout.fillWidth: true
            }
            Button {
                icon.name: "chronometer-reset"
                display: AbstractButton.IconOnly
                text: i18nc("@info:tooltip", "Reset")
                onClicked: root.timer.reset();

                ToolTip.visible: hovered && text.length > 0
                ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                ToolTip.text: text
            }
            Button {
                icon.name: "edit-entry"
                display: AbstractButton.IconOnly
                text: i18nc("@info:tooltip", "Edit")
                enabled: !root.timer.running
                onClicked: root.editClicked()

                ToolTip.visible: hovered && text.length > 0
                ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                ToolTip.text: text
            }
            Item {
                Layout.preferredWidth: buttonLayout.spacing
            }
            Button {
                icon.name: "edit-delete-remove-symbolic"
                display: AbstractButton.IconOnly
                text: i18nc("@info:tooltip", "Delete")
                onClicked: TimerModel.remove(root.timer)

                ToolTip.visible: hovered && text.length > 0
                ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                ToolTip.text: text
            }
        }

        RowLayout {
            Layout.fillWidth: true

            visible: visibleChildren.length > 0

            Kirigami.Icon {
                source: "dialog-scripts"
                visible: root.commandTimeout.length > 0
                implicitWidth: Kirigami.Units.iconSizes.sizeForLabels * 1.5
                implicitHeight: Kirigami.Units.iconSizes.sizeForLabels * 1.5
                color: Kirigami.Theme.disabledTextColor
            }
            Label {
                visible: root.commandTimeout.length > 0
                font.family: "Monospace"
                Layout.fillWidth: true
                text: root.commandTimeout
                color: Kirigami.Theme.disabledTextColor
            }
        }
    }
}
