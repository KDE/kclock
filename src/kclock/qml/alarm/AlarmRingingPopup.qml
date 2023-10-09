/*
 * Copyright 2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import "../components"
import "../components/formatUtil.js" as FormatUtil
import kclock

Kirigami.Dialog {
    id: root
    
    property Alarm alarm
    
    readonly property string formattedTime: alarm ? alarm.formattedTime : ""
    readonly property string name: alarm ? alarm.name : ""

    padding: Kirigami.Units.gridUnit * 3
    
    title: i18n("Alarm is ringing")
    showCloseButton: false
    closePolicy: Popup.NoAutoClose
    
    ColumnLayout {
        width: implicitWidth
        implicitWidth: Kirigami.Units.gridUnit * 20
        Label {
            Layout.alignment: Qt.AlignHCenter
            font.weight: Font.Light
            font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 2)
            text: root.formattedTime
        }
        
        Label {
            Layout.alignment: Qt.AlignHCenter
            font.weight: Font.Bold
            font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 1.25)
            text: root.name ? root.name : i18n("Alarm")
            wrapMode: Text.Wrap
        }
    }
    
    standardButtons: Kirigami.Dialog.NoButton
    flatFooterButtons: true
    
    customFooterActions: [
        Kirigami.Action {
            text: i18n("Snooze")
            onTriggered: alarm.snooze()
        },
        Kirigami.Action {
            text: i18n("Dismiss")
            onTriggered: {
                alarm.dismiss();
            }
        }
    ]
}
