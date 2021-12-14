/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2

import org.kde.kirigami 2.12 as Kirigami

import "../components"
import kclock 1.0

Kirigami.ScrollablePage {
    id: root
    
    property int yTranslate: 0
    
    property bool editMode: false
    
    title: i18n("Alarms")
    icon.name: "notifications"
    
    mainAction: Kirigami.Action {
        iconName: "list-add"
        text: i18n("New Alarm")
        onTriggered: root.addAlarm()
        visible: !Kirigami.Settings.isMobile
    }
    
    actions.contextualActions: [
        Kirigami.Action {
            iconName: "edit-entry"
            text: i18n("Edit")
            onTriggered: root.editMode = !root.editMode
            checkable: true
            visible: alarmsList.count > 0
        },
        Kirigami.Action {
            displayHint: Kirigami.Action.IconOnly
            visible: !applicationWindow().isWidescreen
            iconName: "settings-configure"
            text: i18n("Settings")
            onTriggered: applicationWindow().pageStack.layers.push(applicationWindow().getPage("Settings"))
        }
    ]

    function addAlarm() {
        applicationWindow().pageStack.layers.push(Qt.resolvedUrl("AlarmFormPage.qml"));
    }
    
    header: ColumnLayout {
        Kirigami.InlineMessage {
            type: Kirigami.MessageType.Error
            text: i18n("The clock daemon was not found. Please start kclockd in order to have alarm functionality.")
            visible: !alarmModel.connectedToDaemon // by default, it's false so we need this
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.smallSpacing
        }
    }
    
    ListView {
        id: alarmsList
        model: alarmModel
        currentIndex: -1 // no default selection
        
        transform: Translate { y: yTranslate }
        
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
                iconName: "list-add"
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
            anchors.fill: parent
            iconName: "list-add"
            onClicked: root.addAlarm()
            visible: Kirigami.Settings.isMobile
        }
        
        // each alarm
        delegate: AlarmListDelegate {
            alarm: modelData
            editMode: root.editMode
            width: alarmsList.width
            
            onEditClicked: {
                applicationWindow().pageStack.layers.push(Qt.resolvedUrl("AlarmFormPage.qml"), { selectedAlarm: alarm })
            }
            onDeleteClicked: {
                showPassiveNotification(i18n("Deleted %1", alarm.name == "" ? i18n("alarm") : alarm.name));
                alarmModel.remove(index);
            }
        }
    }
}
