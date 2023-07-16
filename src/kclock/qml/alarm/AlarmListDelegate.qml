/*
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2023 Nate Graham <nate@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.20 as Kirigami

import "../components/formatUtil.js" as FormatUtil

import kclock 1.0

Kirigami.BasicListItem {
    id: root
    
    property Alarm alarm
    
    // avoid null errors when deleting delegate
    readonly property bool enabled: alarm ? alarm.enabled : false
    readonly property string name: alarm ? alarm.name : ""
    readonly property string formattedTime: alarm ? alarm.formattedTime : ""
    readonly property int daysOfWeek: alarm ? alarm.daysOfWeek : 0
    readonly property int snoozedLength: alarm ? alarm.snoozedLength : 0
    
    property bool editMode: false
    
    signal editClicked()
    signal deleteClicked()
    
    onClicked: editClicked()
    
    leftPadding: Kirigami.Units.largeSpacing * 2
    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing
    rightPadding: Kirigami.Units.largeSpacing
    
    // alarm ringing popup
    Loader {
        id: popupLoader
        active: false
        sourceComponent: AlarmRingingPopup {
            alarm: root.alarm
            onVisibleChanged: {
                if (!visible) {
                    popupLoader.active = false;
                }
            }
        }
        
        Component.onCompleted: determineState()
        function determineState() {
            if (root.alarm.ringing) {
                popupLoader.active = true;
                popupLoader.item.open();
            } else if (popupLoader.item) {
                popupLoader.item.close();
            }
        }
        
        Connections {
            target: root.alarm
            ignoreUnknownSignals: true
            
            function onRingingChanged() {
                popupLoader.determineState();
            }
        }
    }

    label: root.formattedTime
    subtitle: {
        let subtitleString = ""
        if (root.name.length > 0) {
            subtitleString = subtitleString + root.name + " - ";
        }

        subtitleString += FormatUtil.getRepeatFormat(root.daysOfWeek);

        if (root.snoozedLength > 0) {
            subtitleString += "\n"
            subtitleString += i18np("Snoozed %1 minute", "Snoozed %1 minutes", root.snoozedLength);
        }
        return subtitleString;
    }

    font.bold: true
    fadeContent: !root.enabled

    trailing: RowLayout {
        spacing: Kirigami.Units.smallSpacing
        
        Switch {
            id: toggleSwitch
            
            // can't use Connections since it conflicts with enabled property
            property bool alarmEnabled: root.enabled
            onAlarmEnabledChanged: checked = root.enabled
            
            checked: root.enabled
            onCheckedChanged: root.alarm.enabled = checked;
        }
        
        ToolButton {
            icon.name: "delete"
            text: i18n("Delete")
            onClicked: root.deleteClicked()
            visible: root.editMode
            display: AbstractButton.IconOnly
            
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.timeout: 5000
            ToolTip.visible: Kirigami.Settings.tabletMode ? pressed : hovered
            ToolTip.text: text
        }
    }
}
