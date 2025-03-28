/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes

import org.kde.kirigami as Kirigami

import "../components"
import kclock

Kirigami.ScrollablePage {
    id: root

    property real yTranslate: 0

    property int clockRadius: Kirigami.Units.gridUnit * 4

    title: i18n("Time")
    readonly property string hiddenTitle: KClockFormat.currentTime
    icon.name: "clock"

    actions: [newAction, editAction, settingsAction]

    function openAddSheet() {
        timeZoneSelect.open();
    }

    Kirigami.Action {
        id: editAction
        icon.name: "edit-entry"
        text: i18n("Edit")
        checkable: true
        visible: listView.count > 0
        onVisibleChanged: {
            if (!visible) {
                checked = false;
            }
        }
    }

    Kirigami.Action {
        id: newAction
        icon.name: "list-add"
        text: i18n("Add")
        visible: !Kirigami.Settings.isMobile
        onTriggered: root.openAddSheet()
    }

    Kirigami.Action {
        id: settingsAction
        displayHint: Kirigami.DisplayHint.IconOnly
        visible: !applicationWindow().isWidescreen
        icon.name: "settings-configure"
        text: i18n("Settings")
        onTriggered: applicationWindow().pageStack.push(applicationWindow().getPage("Settings"))
    }

    AddLocationWrapper {
        id: timeZoneSelect
        active: false
    }

    // time zones
    ListView {
        id: listView
        model: SavedLocationsModel
        currentIndex: -1 // no default selection
        transform: Translate { y: yTranslate }

        add: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: Kirigami.Units.shortDuration }
        }
        remove: Transition {
            NumberAnimation { property: "opacity"; from: 1; to: 0; duration: Kirigami.Units.shortDuration }
        }
        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: Kirigami.Units.longDuration; easing.type: Easing.InOutQuad }
        }

        header: RowLayout {
            width: listView.width - (listView.ScrollBar.vertical ? listView.ScrollBar.vertical.width : 0)
            height: clockItemLoader.height + Kirigami.Units.gridUnit

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.maximumWidth: Kirigami.Units.gridUnit * 23

                // left side - analog clock
                Loader {
                    id: clockItemLoader

                    Layout.alignment: Qt.AlignHCenter
                    width: Math.round(clockRadius * 2 + Kirigami.Units.gridUnit * 0.5)
                    height: clockRadius * 2 + Kirigami.Units.gridUnit

                    // reload clock when page opens for animation
                    asynchronous: true
                    Connections {
                        target: applicationWindow().pageStack
                        function onCurrentItemChanged(): void {
                            clockItemLoader.active = applicationWindow().pageStack.currentItem == applicationWindow().getPage("Time");
                        }
                    }

                    // clock item
                    sourceComponent: Item {
                        id: clockItem
                        AnalogClock {
                            id: analogClock
                            anchors.centerIn: parent
                            height: clockRadius * 2
                            width: clockRadius * 2
                        }
                    }
                }

                Item { Layout.fillWidth: true }

                // right side - digital clock + location
                ColumnLayout {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.rightMargin: Kirigami.Units.gridUnit
                    Label {
                        id: clockLabel
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                        horizontalAlignment: Text.AlignHCenter
                        font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 3)
                        font.weight: Font.Normal
                        opacity: 0.7
                        text: KClockFormat.currentTime
                        color: Kirigami.Theme.textColor
                    }
                    Label {
                        Layout.alignment: Qt.AlignRight
                        font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 1)
                        text: UtilModel.tzName
                        font.weight: Font.Bold
                        opacity: 0.9
                    }
                }
            }
        }

        // backdrop
        Kirigami.Icon {
            opacity: 0.1
            implicitWidth: Math.min(root.height, root.width)
            implicitHeight: Math.min(root.height, root.width)
            source: 'globe'
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.rightMargin: -root.width * 0.2
            anchors.bottomMargin: -Math.min(root.height, root.width) * 0.4
        }

        // mobile action
        FloatingActionButton {
            text: i18nc("@action:button", "Add Location")
            icon.name: "globe"
            onClicked: root.openAddSheet()
            visible: Kirigami.Settings.isMobile
        }

        delegate: TimePageDelegate {
            width: listView.width
            editMode: editAction.checked

            city: model.city
            relativeTime: model.relativeTime
            timeString: model.timeString

            onDeleteRequested: SavedLocationsModel.removeLocation(model.index)
        }
    }
}
