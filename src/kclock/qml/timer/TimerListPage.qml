/*
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick

import org.kde.kirigami as Kirigami

import org.kde.kclock

Kirigami.ScrollablePage {
    id: root

    property real yTranslate: 0

    objectName: "Timers"
    title: i18n("Timers")
    readonly property string hiddenTitle: TimerModel.runningTimer?.remainingPretty ?? ""
    icon.name: "player-time"
    background: null

    // desktop action
    actions: [
        Kirigami.Action {
            icon.name: "list-add"
            text: i18n("New Timer")
            onTriggered: root.addTimer()
            visible: !Kirigami.Settings.isMobile
        },
        Kirigami.Action {
            displayHint: Kirigami.DisplayHint.IconOnly
            visible: !applicationWindow().isWidescreen
            icon.name: "settings-configure"
            text: i18n("Settings")
            onTriggered: applicationWindow().pageStack.push(applicationWindow().getPage("Settings"))
        }
    ]

    function addTimer() : void {
        timerFormDialog.timer = null;
        timerFormDialog.open();
    }

    function editTimer(timer : Timer) : void {
        timerFormDialog.timer = timer;
        timerFormDialog.open();
    }

    function openTimer(timer : Timer) : void {
        applicationWindow().pageStack.push(timerPageComponent, {
            timer
        });
    }

    header: Kirigami.InlineMessage {
        type: Kirigami.MessageType.Error
        text: i18n("The clock daemon was not found. Please start kclockd in order to have timer functionality.")
        visible: !TimerModel.connectedToDaemon // by default, it's false so we need this
        position: Kirigami.InlineMessage.Position.Header
    }

    Component {
        id: timerPageComponent
        TimerPage {}
    }

    ListView {
        id: timersList
        spacing: Kirigami.Units.gridUnit
        topMargin: Kirigami.Units.gridUnit
        bottomMargin: Kirigami.Units.gridUnit
        leftMargin: Kirigami.Units.gridUnit
        rightMargin: Kirigami.Units.gridUnit

        model: TimerModel

        transform: Translate { y: root.yTranslate }

        // TODO: these animations seem to cause the cards to overlap when a new timer is added, possible Qt bug?
        // add: Transition {
        //     NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: Kirigami.Units.shortDuration }
        // }
        // remove: Transition {
        //     NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: Kirigami.Units.shortDuration }
        // }
        // displaced: Transition {
        //     NumberAnimation { properties: "x,y"; duration: Kirigami.Units.longDuration; easing.type: Easing.InOutQuad}
        // }

        // mobile action
        FloatingActionButton {
            text: i18nc("@action:button", "New Timer")
            icon.name: "list-add"
            onClicked: root.addTimer()
            visible: Kirigami.Settings.isMobile
        }

        // no timer placeholder
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: timersList.count === 0
            text: i18n("No timers configured")
            icon.name: "player-time"

            helpfulAction: Kirigami.Action {
                icon.name: "list-add"
                text: i18n("Add timer")
                onTriggered: root.addTimer()
            }
        }

        // create timer form
        TimerFormDialog {
            id: timerFormDialog
        }

        // timer card delegate
        delegate: TimerListDelegate {
            timer: model.timer

            onClicked: root.openTimer(timer)
            onEditClicked: root.editTimer(timer)
        }
    }
}
