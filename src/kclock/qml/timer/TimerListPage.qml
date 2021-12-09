/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
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

Kirigami.ScrollablePage {
    id: timerPage
    
    property real yTranslate: 0
    
    title: i18n("Timers")
    icon.name: "player-time"
    property bool createdTimer: false
    
    // desktop action
    mainAction: Kirigami.Action {
        iconName: "list-add"
        text: i18n("New Timer")
        onTriggered: timerPage.addTimer()
        visible: !Kirigami.Settings.isMobile
    }
    
    actions.contextualActions: [
        Kirigami.Action {
            displayHint: Kirigami.Action.IconOnly
            visible: !applicationWindow().isWidescreen
            iconName: "settings-configure"
            text: i18n("Settings")
            onTriggered: applicationWindow().pageStack.layers.push(applicationWindow().getPage("Settings"))
        }
    ]
    
    function addTimer() {
        newTimerForm.active = true;
        newTimerForm.item.open();
    }
    
    header: ColumnLayout {
        Kirigami.InlineMessage {
            type: Kirigami.MessageType.Error
            text: i18n("The clock daemon was not found. Please start kclockd in order to have timer functionality.")
            visible: !timerModel.connectedToDaemon // by default, it's false so we need this
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.smallSpacing
        }
    }
    
    Kirigami.CardsListView {
        id: timersList
        model: timerModel
        
        transform: Translate { y: yTranslate }
        
        // mobile action
        FloatingActionButton {
            anchors.fill: parent
            iconName: "list-add"
            onClicked: timerPage.addTimer()
            visible: Kirigami.Settings.isMobile
        }
        
        // no timer placeholder
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: timersList.count === 0
            text: i18n("No timers configured")
            icon.name: "player-time"
            
            helpfulAction: Kirigami.Action {
                iconName: "list-add"
                text: i18n("Add timer")
                onTriggered: timerPage.addTimer()
            }
        }
        
        // create timer form
        TimerFormWrapper {
            id: newTimerForm
            active: false
        }
        
        // timer card delegate
        delegate: Kirigami.AbstractCard {
            
            property Timer timerDelegate: timerModel.get(index)
            
            Loader {
                id: timerPageLoader
                active: false
                sourceComponent: TimerPage {
                    id: timerPage
                    timerIndex: index
                    timer: timerDelegate
                    visible: false
                }
            }
            
            showClickFeedback: true
            onClicked: {
                timerPageLoader.active = true;
                applicationWindow().pageStack.layers.push(timerPageLoader.item);
            }
                
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
                        property real progress: timerDelegate ? (timerDelegate.elapsed / timerDelegate.length) : 0
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
                            text: timerDelegate ? timerDelegate.elapsedPretty : ""
                            color: timerDelegate && timerDelegate.running ? Kirigami.Theme.activeTextColor : Kirigami.Theme.disabledTextColor
                        }
                        Kirigami.Heading {
                            level: 3
                            Layout.alignment: Qt.AlignRight
                            text: timerDelegate ? timerDelegate.lengthPretty : ""
                            color: timerDelegate && timerDelegate.running ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                        }
                    }
                    
                    RowLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        Kirigami.Heading {
                            level: 4
                            Layout.alignment: Qt.AlignLeft
                            text: timerDelegate ? timerDelegate.label : ""
                        }
                        
                        Row {
                            Layout.alignment: Qt.AlignRight
                            ToolButton {
                                icon.name: timerDelegate && timerDelegate.looping ? "media-repeat-all" : "media-repeat-none"
                                display: AbstractButton.IconOnly
                                text: i18n("Loop Timer")
                                checked: timerDelegate && timerDelegate.looping
                                onClicked: timerDelegate.toggleLooping()
                                
                                ToolTip.visible: hovered && text.length > 0
                                ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                                ToolTip.text: text
                            }
                            ToolButton {
                                icon.name: timerDelegate && timerDelegate.running ? "chronometer-pause" : "chronometer-start"
                                display: AbstractButton.IconOnly
                                text: timerDelegate && timerDelegate.running ? i18n("Pause") : i18n("Start")
                                onClicked: timerDelegate.toggleRunning()
                                
                                ToolTip.visible: hovered && text.length > 0
                                ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                                ToolTip.text: text
                            }
                            ToolButton {
                                icon.name: "chronometer-reset"
                                display: AbstractButton.IconOnly
                                text: i18n("Reset")
                                onClicked: timerDelegate.reset();
                                
                                ToolTip.visible: hovered && text.length > 0
                                ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
                                ToolTip.text: text
                            }
                            ToolButton {
                                icon.name: "delete"
                                display: AbstractButton.IconOnly
                                text: i18n("Delete")
                                onClicked: timerModel.remove(index)
                                
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
                            visible: timerDelegate && timerDelegate.commandTimeout.length > 0
                            implicitWidth: Kirigami.Units.iconSizes.sizeForLabels * 1.5
                            implicitHeight: Kirigami.Units.iconSizes.sizeForLabels * 1.5
                            color: timerDelegate && Kirigami.Theme.disabledTextColor
                        }
                        Label {
                            visible: timerDelegate && timerDelegate.commandTimeout.length > 0
                            font.family: "Monospace"
                            Layout.fillWidth: true
                            text: timerDelegate.commandTimeout
                            color: timerDelegate && Kirigami.Theme.disabledTextColor
                        }
                    }
                }
            }
        }
    }
}
