/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.12 as Kirigami

import "../components/formatUtil.js" as FormatUtil

import kclock 1.0

Kirigami.SwipeListItem {
    id: root
    
    property Alarm alarm
    
    // avoid null errors when deleting delegate
    readonly property bool enabled: alarm ? alarm.enabled : false
    readonly property string name: alarm ? alarm.name : ""
    readonly property int hours: alarm ? alarm.hours : 0
    readonly property int minutes: alarm ? alarm.minutes : 0
    readonly property int daysOfWeek: alarm ? alarm.daysOfWeek : 0
    readonly property int snoozedLength: alarm ? alarm.snoozedLength : 0
    
    signal editClicked()
    signal deleteClicked()
    
    leftPadding: Kirigami.Units.largeSpacing * 2
    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing
    
    onClicked: alarm.enabled = !alarm.enabled
    
    actions: [
        Kirigami.Action {
            iconName: "entry-edit"
            text: i18n("Edit")
            onTriggered: root.editClicked()
        },
        Kirigami.Action {
            iconName: "delete"
            text: i18n("Delete")
            onTriggered: root.deleteClicked();
        }
    ]

    // alarm text
    contentItem: Item {
        implicitWidth: delegateLayout.implicitWidth
        implicitHeight: delegateLayout.implicitHeight

        GridLayout {
            id: delegateLayout
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.right: parent.right

            rowSpacing: Kirigami.Units.smallSpacing
            columnSpacing: Kirigami.Units.smallSpacing
            columns: width > Kirigami.Units.gridUnit * 20 ? 4 : 2

            ColumnLayout {
                Label {
                    font.weight: Font.Light
                    font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 1.75)
                    text: kclockFormat.formatTimeString(root.hours, root.minutes)
                    color: root.enabled ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                }
                
                RowLayout {
                    spacing: 0
                    Label {
                        id: alarmName
                        visible: text !== ""
                        font.weight: Font.Bold
                        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.15
                        color: root.enabled ? Kirigami.Theme.activeTextColor : Kirigami.Theme.disabledTextColor
                        text: root.name
                    }
                    Label {
                        font.weight: Font.Normal
                        text: (alarmName.visible ? " - " : "") + FormatUtil.getRepeatFormat(root.daysOfWeek) 
                        color: root.enabled ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                    }
                }
                
                Label {
                    visible: root.snoozedLength != 0
                    font.weight: Font.Bold
                    color: Kirigami.Theme.disabledTextColor
                    text: i18n("Snoozed %1 minutes", root.snoozedLength)
                }
            }

            Switch {
                id: toggleSwitch
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.columnSpan: 1
                
                // can't use Connections since it conflicts with enabled property
                property bool alarmEnabled: root.enabled
                onAlarmEnabledChanged: checked = root.enabled
                
                // prevent binding loop
                property bool selfEdit: false
                checked: root.enabled
                onCheckedChanged: {
                    if (selfEdit) {
                        selfEdit = false;
                    } else {
                        root.alarm.enabled = checked;
                        selfEdit = true;
                    }
                }
            }
        }
    }
}
