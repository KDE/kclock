/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
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

    property bool editMode: false

    title: i18n("Alarms")
    icon.name: "notifications"

    actions: [
        Kirigami.Action {
            icon.name: "list-add"
            text: i18n("New Alarm")
            onTriggered: root.addAlarm()
            visible: !Kirigami.Settings.isMobile
        },
        Kirigami.Action {
            icon.name: "edit-entry"
            text: i18n("Edit")
            onTriggered: root.editMode = !root.editMode
            checkable: true
            visible: alarmsList.count > 0
        },
        Kirigami.Action {
            displayHint: Kirigami.DisplayHint.IconOnly
            visible: !applicationWindow().isWidescreen
            icon.name: "settings-configure"
            text: i18n("Settings")
            onTriggered: applicationWindow().pageStack.push(applicationWindow().getPage("Settings"))
        }
    ]

    function addAlarm() {
        applicationWindow().pageStack.push(Qt.resolvedUrl("AlarmFormPage.qml"));
    }

    header: Kirigami.InlineMessage {
        type: Kirigami.MessageType.Error
        text: i18n("The clock daemon was not found. Please start kclockd in order to have alarm functionality.")
        visible: !AlarmModel.connectedToDaemon // by default, it's false so we need this
        position: Kirigami.InlineMessage.Position.Header
    }

    ListView {
        id: alarmsList
        model: AlarmModel
        currentIndex: -1 // no default selection

        topMargin: Kirigami.Units.smallSpacing
        reuseItems: true

        // no alarms placeholder
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: Kirigami.Units.largeSpacing
            visible: alarmsList.count == 0
            text: i18n("No alarms configured")
            icon.name: "notifications"

            helpfulAction: Kirigami.Action {
                icon.name: "list-add"
                text: i18n("Add alarm")
                onTriggered: root.addAlarm()
            }
        }

        add: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: Kirigami.Units.shortDuration }
        }
        remove: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: Kirigami.Units.shortDuration }
        }
        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: Kirigami.Units.longDuration; easing.type: Easing.InOutQuad}
        }

        // mobile action
        FloatingActionButton {
            icon.name: "list-add"
            onClicked: root.addAlarm()
            visible: Kirigami.Settings.isMobile
        }

        // each alarm
        delegate: AlarmListDelegate {
            alarm: modelData
            editMode: root.editMode
            width: alarmsList.width

            onEditClicked: {
                applicationWindow().pageStack.push(Qt.resolvedUrl("AlarmFormPage.qml"), { selectedAlarm: alarm })
                alarmsList.currentIndex = -1
            }
            onDeleteClicked: {
                showPassiveNotification(i18n("Deleted %1", alarm.name == "" ? i18n("alarm") : alarm.name));
                AlarmModel.remove(index);
            }
        }
    }
}
