/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *                Han Young <hanyoung@protonmail.com>
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
import kclock 1.0

Kirigami.ScrollablePage {

    property Alarm selectedAlarm: null
    property int alarmDaysOfWeek: selectedAlarm.daysOfWeek
    property int indexInList // index in alarm list
    property bool newAlarm: false
    property string ringtonePath: ""

    title: selectedAlarm.name
    
    actions {
        main: Kirigami.Action {
            iconName: "checkmark"
            text: i18n("Done")
            onTriggered: {
                let hours = selectedAlarmTime.hours + (selectedAlarmTime.pm ? 12 : 0);
                let minutes = selectedAlarmTime.minutes;
                selectedAlarm.hours = hours;
                selectedAlarm.minutes = minutes;
                selectedAlarm.daysOfWeek = alarmDaysOfWeek;
                selectedAlarm.save();
                if(newAlarm)
                    alarmModel.addNewAlarm();
                else
                    alarmModel.dataChanged(indexInList, indexInList);
                selectedAlarm.stop();
                pageStack.pop();
            }
        }
    }

    Column {
        spacing: Kirigami.Units.largeSpacing

        // time picker
        DateAndTime.TimePicker {
            id: selectedAlarmTime

            hours: hoursTo12(selectedAlarm.hours)
            minutes: selectedAlarm.minutes
            pm: selectedAlarm.hours > 12

            height: 400
            anchors.horizontalCenter: parent.horizontalCenter
            width: Math.min(400, parent.width)
        }

        Kirigami.Separator {
            width: parent.width
        }

        // repeat day picker
        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: i18n("Days to Repeat")
            font.weight: Font.Bold
        }

        Flow {
            anchors.horizontalCenter: parent.horizontalCenter
            Repeater {
                model: ListModel {
                    id: selectedDays
                    ListElement { displayText: "S"; dayFlag: 64 }
                    ListElement { displayText: "M"; dayFlag: 1 }
                    ListElement { displayText: "T"; dayFlag: 2 }
                    ListElement { displayText: "W"; dayFlag: 4 }
                    ListElement { displayText: "T"; dayFlag: 8 }
                    ListElement { displayText: "F"; dayFlag: 16 }
                    ListElement { displayText: "S"; dayFlag: 32 }
                }

                ToolButton {
                    implicitWidth: 40
                    text: displayText
                    checkable: true
                    checked: (alarmDaysOfWeek & dayFlag) == dayFlag
                    highlighted: false
                    onClicked: {
                        if (checked) alarmDaysOfWeek |= dayFlag;
                        else alarmDaysOfWeek &= ~dayFlag;
                    }
                }
            }
        }

        Kirigami.Separator {
            width: parent.width
        }

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: i18n("Alarm Name")
            font.weight: Font.Bold
        }
        TextField {
            anchors.horizontalCenter: parent.horizontalCenter
            id: selectedAlarmName
            placeholderText: i18n("Wake Up")
            text: selectedAlarm.name
            onTextChanged: selectedAlarm.name = this.text
        }
        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: i18n("Ringtone")
            font.weight: Font.Bold
        }

        Kirigami.ActionTextField {
            id: selectAlarmField
            anchors.horizontalCenter: parent.horizontalCenter
            placeholderText: selectedAlarm.ringtoneName

            rightActions: [
                Kirigami.Action {
                    iconName: "list-add"
                    onTriggered: {
                        ringtonePath = selectedAlarm.selectRingtone();
                        if(ringtonePath != ""){
                            selectedAlarm.ringtonePath = ringtonePath;
                            selectedAlarm.ringtoneName = ringtonePath.toString().split('/').pop();
                        }
                    }
                }
            ]
        }

        RowLayout {
            anchors.horizontalCenter: parent.horizontalCenter
            width: selectAlarmField.width
            Text {
                text: i18n("Volume: ")
            }
            Slider {
                Layout.fillWidth: true
                from: 0
                to: 100
                value: selectedAlarm.volume // this doesn't auto update Cpp value
                onValueChanged: selectedAlarm.volume = value
                onPressedChanged: {
                    if(!pressed){
                        selectedAlarm.play();
                    }
                }
            }
        }
    }
    function hoursTo12(hours){ // auxiliary function to convert 24hours to 12
        if(hours >= 12)
            return hours - 12;
        else
            return hours;
    }
}

