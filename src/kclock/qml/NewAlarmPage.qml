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
import QtQuick.Dialogs 1.3
import org.kde.kirigami 2.11 as Kirigami
import org.kde.kirigamiaddons.dateandtime 0.1 as DateAndTime
import kclock 1.0

Kirigami.ScrollablePage {

    property Alarm selectedAlarm: null
    property int alarmDaysOfWeek: selectedAlarm ? selectedAlarm.daysOfWeek : 0
    property string ringtonePath: selectedAlarm ? selectedAlarm.ringtonePath : ""

    id: newAlarmPageRoot
    title: selectedAlarm ? selectedAlarm.name : i18n("New Alarm")
    
    actions {
        main: Kirigami.Action {
            iconName: "checkmark"
            text: i18n("Done")
            onTriggered: {
                let hours = selectedAlarmTime.hours + (selectedAlarmTime.pm ? 12 : 0);
                let minutes = selectedAlarmTime.minutes;

                if (selectedAlarm) {
                    selectedAlarm.name = selectedAlarmName.text;
                    selectedAlarm.hours = hours;
                    selectedAlarm.minutes = minutes;
                    selectedAlarm.daysOfWeek = alarmDaysOfWeek;
                    selectedAlarm.save(); // remote save
                    alarmPlayer.stop();
                    if (selectedAlarm.enabled) {
                        showPassiveNotification(selectedAlarm.timeToRingFormated());
                    }
                } else {
                    alarmModel.addAlarm(hours, minutes, alarmDaysOfWeek, selectedAlarmName.text);
                    alarmPlayer.stop();
                    showPassiveNotification(alarmModel.timeToRingFormated(hours, minutes, alarmDaysOfWeek));
                }
                pageStack.layers.pop();
            }
        }
    }

    Column {
        spacing: Kirigami.Units.largeSpacing

        // time picker
        DateAndTime.TimePicker {
            id: selectedAlarmTime

            hours: selectedAlarm ? hoursTo12(selectedAlarm.hours) : 0
            minutes: selectedAlarm ? selectedAlarm.minutes : 0
            pm: selectedAlarm ? selectedAlarm.hours >= 12 : false
            
            Component.onCompleted: {
                if (!selectedAlarm) { // new alarm
                    let date = new Date();
                    hours = date.getHours() >= 12 ? date.getHours() - 12 : date.getHours();
                    minutes = date.getMinutes();
                    pm = date.getHours() >= 12;
                }
            }

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
                model: weekModel

                ToolButton {
                    implicitWidth: 40
                    text: name
                    checkable: true
                    checked: kclockFormat.isChecked(index, alarmDaysOfWeek)
                    highlighted: false
                    onClicked: {
                        if (checked) alarmDaysOfWeek |= flag;
                        else alarmDaysOfWeek &= ~flag;
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
            text: selectedAlarm ? selectedAlarm.name : "Alarm"
        }
        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: i18n("Ringtone")
            font.weight: Font.Bold
        }

        Kirigami.ActionTextField {
            id: selectAlarmField
            anchors.horizontalCenter: parent.horizontalCenter
            placeholderText: selectedAlarm ? selectedAlarm.ringtoneName : ""
            width: newAlarmPageRoot.width * 0.8
            rightActions: [
                Kirigami.Action {
                    iconName: "list-add"
                    onTriggered: {
                        fileDialog.open();
                    }
                }
            ]
        }

        FileDialog {
            id: fileDialog
            title: "Choose an audio"
            folder: shortcuts.music
            onAccepted: {
                newAlarmPageRoot.ringtonePath = fileDialog.fileUrl;
                if (ringtonePath != "") {
                    if (selectedAlarm) {
                        selectedAlarm.ringtonePath = newAlarmPageRoot.ringtonePath;
                        selectedAlarm.ringtoneName = newAlarmPageRoot.ringtonePath.toString().split('/').pop();
                    }
                    console.log(newAlarmPageRoot.ringtonePath);
                    alarmPlayer.setSource(newAlarmPageRoot.ringtonePath);
                    alarmPlayer.play();
                }
                this.close();
            }
            onRejected: {
                this.close();
            }
            nameFilters: [ "Audio files (*.wav *.mp3 *.ogg *.aac *.flac *.webm *.mka *.opus)", "All files (*)" ]
        }
    }
    function hoursTo12(hours){ // auxiliary function to convert 24hours to 12
        if(hours >= 12)
            return hours - 12;
        else
            return hours;
    }
}

