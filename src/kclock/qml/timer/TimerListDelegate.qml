/*
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2

import org.kde.kirigami 2.15 as Kirigami

import "../components"
import kclock 1.0

Kirigami.AbstractCard {
    id: root
    property Timer timer
    
    readonly property int length: timer ? timer.length : 1
    readonly property int elapsed: timer ? timer.elapsed : 0
    readonly property string lengthPretty: timer ? timer.lengthPretty : ""
    readonly property string elapsedPretty: timer ? timer.elapsedPretty : ""
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
        function determineState() {
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
                    text: root.elapsedPretty
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
                Kirigami.Heading {
                    level: 4
                    Layout.alignment: Qt.AlignLeft
                    text: root.label
                }
                
                Row {
                    Layout.alignment: Qt.AlignRight
                    ToolButton {
                        icon.name: root.looping ? "media-repeat-all" : "media-repeat-none"
                        display: AbstractButton.IconOnly
                        text: i18n("Loop Timer")
                        checked: root.looping
                        onClicked: root.timer.toggleLooping()
                        
                        ToolTip.visible: hovered && text.length > 0
                        ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                        ToolTip.text: text
                    }
                    ToolButton {
                        icon.name: root.running ? "chronometer-pause" : "chronometer-start"
                        display: AbstractButton.IconOnly
                        text: root.running ? i18n("Pause") : i18n("Start")
                        onClicked: root.timer.toggleRunning()
                        
                        ToolTip.visible: hovered && text.length > 0
                        ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                        ToolTip.text: text
                    }
                    ToolButton {
                        icon.name: "chronometer-reset"
                        display: AbstractButton.IconOnly
                        text: i18n("Reset")
                        onClicked: root.timer.reset();
                        
                        ToolTip.visible: hovered && text.length > 0
                        ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                        ToolTip.text: text
                    }
                    ToolButton {
                        icon.name: "delete"
                        display: AbstractButton.IconOnly
                        text: i18n("Delete")
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
