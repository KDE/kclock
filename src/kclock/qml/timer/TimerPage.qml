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

    property Timer timer
    property int timerIndex

    title: timer && timer.label !== "" ? timer.label : i18n("New timer")
    readonly property string hiddenTitle: running ? timer.remainingPretty : ""

    property bool showFullscreen: false

    property int elapsed: timer ? timer.elapsed : 0
    property int duration: timer ? timer.length : 0
    property bool running: timer ? timer.running : 0
    property bool looping: timer ? timer.looping : 0
    property string commandTimeout: timer ? timer.commandTimeout : ""
    property bool isCommandTimeout: commandTimeout.length > 0

    // keyboard controls
    Keys.onSpacePressed: timer.toggleRunning();
    Keys.onReturnPressed: timer.toggleRunning();

    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    // topbar action
    actions: [
        Kirigami.Action {
            text: running ? i18n("Pause") : i18n("Start")
            icon.name: running ? "chronometer-pause" : "chronometer-start"
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
                TimerModel.remove(timerIndex);
            }
        },
        Kirigami.Action {
            icon.name: "media-repeat-all"
            text: i18n("Loop Timer")
            checkable: true
            checked: looping
            visible: !Kirigami.Settings.isMobile
            onTriggered: root.timer.toggleLooping()
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
                    TimerModel.remove(timerIndex);
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

    TimerComponent {
        anchors.fill: parent
        timerDuration: duration
        timerElapsed: elapsed
        timerRunning: running

        onRequestAddMinute: {
            root.timer.addMinute();
        }
    }

    RowLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        Kirigami.Icon {
            source: "dialog-scripts"
            visible: isCommandTimeout
            implicitWidth: Kirigami.Units.iconSizes.sizeForLabels * 1.5
            implicitHeight: Kirigami.Units.iconSizes.sizeForLabels * 1.5
            color: Kirigami.Theme.disabledTextColor
        }
        Label {
            visible: isCommandTimeout
            font.family: "Monospace"
            text: root.commandTimeout
            color: Kirigami.Theme.disabledTextColor
        }
    }
}
