/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.11 as Kirigami
import org.kde.kirigamiaddons.dateandtime 0.1 as DateAndTime
import kirigamiclock 1.0

Kirigami.ScrollablePage {
    title: newAlarm ? i18n("New Alarm") : i18n("Edit Alarm")

    property Alarm selectedAlarm: null
    property bool newAlarm: true
    property int alarmDaysOfWeek: newAlarm ? 0 : selectedAlarm.dayOfWeek
    
    function init(alarm) {
        if (alarm == null) {
            newAlarm = true;
            alarmDaysOfWeek = 0;
        } else {
            newAlarm = false;
            alarmDaysOfWeek = alarm.dayOfWeek;
        }
        selectedAlarm = alarm;
    }
    
    actions {
        main: Kirigami.Action {
            iconName: "checkmark"
            text: i18n("Done")
            onTriggered: {
                let hours = selectedAlarmTime.hours + (selectedAlarmTime.pm ? 12 : 0);
                let minutes = selectedAlarmTime.minutes;
                if (newAlarm) {
                    selectedAlarm = alarmModel.insert(0, selectedAlarmName.text, minutes, hours, alarmDaysOfWeek);
                    newAlarm = false; // reset form
                } else {
                    selectedAlarm.name = selectedAlarmName.text;
                    selectedAlarm.minutes = minutes;
                    selectedAlarm.hours = hours;
                    selectedAlarm.dayOfWeek = alarmDaysOfWeek;
                    
                    // must manually update the UI
                    alarmModel.updateUi();
                }
                // reset
                alarmDaysOfWeek = 0;
                newAlarm = true;
                pageStack.pop()
            }
        }
    }
    
    ColumnLayout {
        spacing: Kirigami.Units.largeSpacing
        
        DateAndTime.TimePicker {
            Layout.alignment: Qt.AlignHCenter
            id: selectedAlarmTime
            hours: newAlarm ? 0 : selectedAlarm.hours
            minutes: newAlarm ? 0 : selectedAlarm.minutes
        }
        
        Kirigami.Separator {
            Layout.fillWidth: true
        }
        
        Item {
            Layout.fillWidth: true
            implicitHeight: Kirigami.Units.gridUnit * 3
            
            ColumnLayout {
                anchors.leftMargin: Kirigami.Units.gridUnit
                anchors.rightMargin: Kirigami.Units.gridUnit
                anchors.fill: parent
                
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: "<b>" + i18n("Days to Repeat") + "</b>"
                }
                Flow {
                    Layout.alignment: Qt.AlignHCenter
                    Repeater {
                        model: ListModel {
                            id: selectedDays
                            ListElement { displayText: "S"; dayFlag: 1 }
                            ListElement { displayText: "M"; dayFlag: 2 }
                            ListElement { displayText: "T"; dayFlag: 4 }
                            ListElement { displayText: "W"; dayFlag: 8 }
                            ListElement { displayText: "T"; dayFlag: 16 }
                            ListElement { displayText: "F"; dayFlag: 32 }
                            ListElement { displayText: "S"; dayFlag: 64 }
                        }
                        
                        Button {
                            implicitWidth: 40
                            text: displayText
                            checkable: true
                            checked: ((newAlarm ? alarmDaysOfWeek : selectedAlarm.dayOfWeek) & dayFlag) == dayFlag
                            highlighted: false
                            onClicked: {
                                if (checked) alarmDaysOfWeek |= dayFlag;
                                else alarmDaysOfWeek &= ~dayFlag;
                            }
                        }
                    }
                }
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }
        
        Item {
            Layout.fillWidth: true
            implicitHeight: Kirigami.Units.gridUnit * 3
            
            ColumnLayout {
                anchors.leftMargin: Kirigami.Units.gridUnit
                anchors.rightMargin: Kirigami.Units.gridUnit
                anchors.fill: parent
                
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: "<b>" + i18n("Alarm Name") + "</b>"
                }
                TextField {
                    Layout.alignment: Qt.AlignHCenter
                    id: selectedAlarmName
                    placeholderText: i18n("Wake Up")
                    text: newAlarm ? "" : selectedAlarm.name
                }
            }
        }
    }
}
