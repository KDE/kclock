/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2

import org.kde.kirigami 2.11 as Kirigami

import "../components"
import kclock 1.0

Kirigami.Page {
    
    property Timer timer
    property int timerIndex
    
    id: timerpage
    title: timer && timer.label !== "" ? timer.label : i18n("New timer")
    
    property bool showFullscreen: false
    
    property int elapsed: timer == null ? 0 : timer.elapsed
    property int duration: timer == null ? 0 : timer.length
    property bool running: timer == null ? 0 : timer.running
    property bool looping: timer == null ? 0 : timer.looping
    property bool isCommandTimeout: timer == null ? 0 : timer.commandTimeout.length > 0

    // keyboard controls
    Keys.onSpacePressed: timer.toggleRunning();
    Keys.onReturnPressed: timer.toggleRunning();
    
    // topbar action
    actions {
        main: Kirigami.Action {
            text: running ? i18n("Pause") : i18n("Start")
            iconName: running ? "chronometer-pause" : "chronometer-start"
            onTriggered: timer.toggleRunning()
        }

        contextualActions: [
            Kirigami.Action {
                icon.name: "chronometer-reset"
                text: i18n("Reset")
                onTriggered: timer.reset();
            },
            Kirigami.Action {
                icon.name: "delete"
                text: i18n("Delete")
                onTriggered: {
                    pageStack.pop();
                    timerModel.remove(timerIndex);
                }
            },
            Kirigami.Action {
                icon.name: "media-repeat-all"
                text: i18n("Loop Timer")
                checkable: true
                checked: looping
                onTriggered: timer.toggleLooping()
            }
        ]
    }
    
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
                text: timerpage.running ? i18n("Pause") : i18n("Start")
                icon.name: timerpage.running ? "chronometer-pause" : "chronometer-start"
                onClicked: timer.toggleRunning()
            }
            FooterToolBarButton {
                display: toolbar.opened ? AbstractButton.TextUnderIcon : AbstractButton.TextOnly
                text: i18n("Reset")
                icon.name: "chronometer-reset"
                onClicked: timer.reset()
            }
            FooterToolBarButton {
                display: toolbar.opened ? AbstractButton.TextUnderIcon : AbstractButton.TextOnly
                text: i18n("Delete")
                icon.name: "delete"
                onClicked: {
                    pageStack.layers.pop();
                    timerModel.remove(timerIndex);
                }
            }
            FooterToolBarButton {
                display: toolbar.opened ? AbstractButton.TextUnderIcon : AbstractButton.TextOnly
                text: i18n("Loop Timer")
                icon.name: "media-repeat-all"
                checked: timerpage.looping
                onClicked: timer.toggleLooping()
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

    TimerComponent {
        timerDuration: duration
        timerElapsed: elapsed
        timerRunning: running
    }
    
    RowLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        Kirigami.Icon {
            source: "dialog-scripts"
            visible: isCommandTimeout
            implicitWidth: Kirigami.Units.iconSizes.sizeForLabels * 1.5
            implicitHeight: Kirigami.Units.iconSizes.sizeForLabels * 1.5
            color: timerDelegate && Kirigami.Theme.disabledTextColor
        }
        Label {
            visible: isCommandTimeout
            font.family: "Monospace"
            text: timerDelegate.commandTimeout
            color: timerDelegate && Kirigami.Theme.disabledTextColor
        }
    }
}