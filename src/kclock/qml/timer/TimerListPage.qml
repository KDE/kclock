/*
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import "../components"
import kclock

Kirigami.ScrollablePage {
    id: root

    property real yTranslate: 0

    title: i18n("Timers")
    readonly property string hiddenTitle: TimerModel.runningTimer?.remainingPretty ?? ""
    icon.name: "player-time"

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

    function addTimer() {
        timerFormDialog.timer = null;
        timerFormDialog.open();
    }

    function editTimer(timer) {
        timerFormDialog.timer = timer;
        timerFormDialog.open();
    }

    header: Kirigami.InlineMessage {
        type: Kirigami.MessageType.Error
        text: i18n("The clock daemon was not found. Please start kclockd in order to have timer functionality.")
        visible: !TimerModel.connectedToDaemon // by default, it's false so we need this
        position: Kirigami.InlineMessage.Position.Header
    }

    ListView {
        id: timersList
        spacing: Kirigami.Units.gridUnit
        topMargin: Kirigami.Units.gridUnit
        bottomMargin: Kirigami.Units.gridUnit
        leftMargin: Kirigami.Units.gridUnit
        rightMargin: Kirigami.Units.gridUnit

        model: TimerModel

        transform: Translate { y: yTranslate }

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

            onEditClicked: root.editTimer(timer)
        }
    }
}
