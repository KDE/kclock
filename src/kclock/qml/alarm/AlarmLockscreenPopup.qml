/*
 * Copyright 2025 Tushar Gupta <tushar.197712@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kclock

Window {
    id: root
    objectName: "AlarmLockscreenPopup"

    property string alarmPopupId
    property Alarm alarm

    readonly property string formattedTime: alarm ? alarm.formattedTime : ""
    readonly property string name: alarm ? alarm.name : ""
    
    title: i18n("Alarm is ringing")
    visible: true
    visibility: Window.FullScreen
    color: Kirigami.Theme.backgroundColor
    
    Connections {
        target: AlarmModel
        function onStatusChanged(status) {
            alarm = AlarmModel.alarmByUuid(alarmPopupId);
        }
    }
    
    
    ColumnLayout {
        anchors.centerIn: parent
        spacing: Kirigami.Units.largeSpacing * 2
        width: implicitWidth
        
        Label {
            Layout.alignment: Qt.AlignHCenter
            font.weight: Font.Light
            font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 6)
            text: root.formattedTime
        }
        
        Label {
            Layout.alignment: Qt.AlignHCenter
            font.weight: Font.Bold
            font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 1.5)
            text: root.name ? root.name : i18n("Alarm not found")
            wrapMode: Text.Wrap
        }
        
        Item {
            Layout.preferredHeight: Kirigami.Units.gridUnit * 2
        }
        
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: Kirigami.Units.largeSpacing * 2
            
            Button {
                id: snoozeButton
                Layout.preferredWidth: Kirigami.Units.gridUnit * 12
                Layout.preferredHeight: Kirigami.Units.gridUnit * 3
                focusPolicy: Qt.StrongFocus
                
                background: Rectangle {
                    color: snoozeButton.pressed ? Qt.darker("#2c5d7a", 1.2) : "#2c5d7a"
                    radius: height / 2
                }
                
                contentItem: Label {
                    text: i18n("Snooze")
                    font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 1.5)
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: {
                    if (root.alarm) {
                        root.alarm.snooze();
                    }
                    root.close()
                }
            }
            
            Button {
                id: stopButton
                Layout.preferredWidth: Kirigami.Units.gridUnit * 12
                Layout.preferredHeight: Kirigami.Units.gridUnit * 3
                focusPolicy: Qt.StrongFocus
                
                background: Rectangle {
                    color: stopButton.pressed ? Qt.darker("#4169b8", 1.2) : "#4169b8"
                    radius: height / 2
                }
                
                contentItem: Label {
                    text: i18n("Stop")
                    font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 1.5)
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: {
                    if (root.alarm) {
                        root.alarm.dismiss();
                    }
                    root.close()
                }
                
            }
        }
    }
    
}
