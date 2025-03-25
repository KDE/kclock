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
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: Kirigami.Units.shortDuration }
        }
        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: Kirigami.Units.longDuration; easing.type: Easing.InOutQuad }
        }

        header: Control {
            width: listView.width - (listView.ScrollBar.vertical ? listView.ScrollBar.vertical.width : 0)
            height: Kirigami.Units.gridUnit * 9

            topPadding: 0
            bottomPadding: 0
            leftPadding: 0
            rightPadding: 0

            background: Rectangle {
                color: Kirigami.Theme.alternateBackgroundColor
                Kirigami.Separator { anchors.bottom: parent.bottom; anchors.left: parent.left; anchors.right: parent.right }
            }

            contentItem: Item {
                ColumnLayout {
                    anchors.centerIn: parent
                    // TODO: the clock visual doesn't show up for some reason, disable for now
                    Label {
                        id: clockLabel
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                        horizontalAlignment: Text.AlignHCenter
                        font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 3)
                        font.weight: Font.Normal
                        opacity: 0.7
                        text: KClockFormat.currentTime
                        color: Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.textColor, Kirigami.Theme.activeTextColor, 0.5)
                    }
                    Label {
                        height: clockLabel.height // ensure they have the same height for alignment
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                        horizontalAlignment: Text.AlignHCenter
                        font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 1)
                        font.weight: Font.Bold
                        text: UtilModel.tzName
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
