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
    onClicked: {
        timerPageLoader.active = true;
        applicationWindow().pageStack.push(timerPageLoader.item);
    }
    
    // timer page
    Loader {
        id: timerPageLoader
        active: false
        sourceComponent: TimerPage {
            id: timerPage
            timerIndex: index
            timer: root.timer
            visible: false
        }
    }
    
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
    contentItem: Item {
        implicitWidth: delegateLayout.implicitWidth
        implicitHeight: delegateLayout.implicitHeight
        
        Column {
            id: delegateLayout
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            spacing: Kirigami.Units.smallSpacing
            
            ProgressBar {
                anchors.left: parent.left
                anchors.right: parent.right
                property real progress: root.elapsed / root.length
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
                anchors.left: parent.left
                anchors.right: parent.right
                Kirigami.Heading {
                    level: 3
                    Layout.alignment: Qt.AlignLeft
                    text: root.remainingPretty
                    color: root.running ? Kirigami.Theme.activeTextColor : Kirigami.Theme.disabledTextColor
                }
                Kirigami.Heading {
                    level: 3
                    Layout.alignment: Qt.AlignRight
                    text: root.lengthPretty
                    color: root.running ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                }
            }
            
            RowLayout {
                anchors.left: parent.left
                anchors.right: parent.right

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

                Kirigami.Heading {
                    id: label
                    level: 4
                    Layout.fillWidth: true
                    text: root.label
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
                
                Row {
                    Layout.alignment: Qt.AlignRight
                    ToolButton {
                        icon.name: root.running ? "chronometer-pause" : "chronometer-start"
                        display: AbstractButton.IconOnly
                        text: root.running ? i18nc("@info:tooltip", "Pause") : i18nc("@info:tooltip", "Start")
                        onClicked: root.timer.toggleRunning()
                        
                        ToolTip.visible: hovered && text.length > 0
                        ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                        ToolTip.text: text
                    }
                    ToolButton {
                        icon.name: "chronometer-reset"
                        display: AbstractButton.IconOnly
                        text: i18nc("@info:tooltip", "Reset")
                        onClicked: root.timer.reset();
                        
                        ToolTip.visible: hovered && text.length > 0
                        ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                        ToolTip.text: text
                    }
                    ToolButton {
                        icon.name: "edit-entry"
                        display: AbstractButton.IconOnly
                        text: i18nc("@info:tooltip", "Edit")
                        enabled: !root.timer.running
                        onClicked: root.editClicked()

                        ToolTip.visible: hovered && text.length > 0
                        ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                        ToolTip.text: text
                    }
                    ToolButton {
                        icon.name: "delete"
                        display: AbstractButton.IconOnly
                        text: i18nc("@info:tooltip", "Delete")
                        onClicked: TimerModel.remove(index)
                        
                        ToolTip.visible: hovered && text.length > 0
                        ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                        ToolTip.text: text
                    }
                }
            }
            
            RowLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                Layout.alignment: Qt.AlignLeft
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
}
