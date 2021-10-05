/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
    id: root

    property Alarm selectedAlarm: null
    property int alarmDaysOfWeek: selectedAlarm ? selectedAlarm.daysOfWeek : 0
    property string ringtonePath: selectedAlarm ? selectedAlarm.ringtonePath : ""

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false
    
    title: {
        if (selectedAlarm) {
            return i18nc("Edit alarm page title", "Editing %1", selectedAlarm.name === "" ? i18n("Alarm") : selectedAlarm.name);
        } else {
            return i18n("New Alarm");
        }
    }
    
    function formComplete() {
        let hours = selectedAlarmTime.hours + (selectedAlarmTime.pm ? 12 : 0);
        let minutes = selectedAlarmTime.minutes;

        if (selectedAlarm) {
            selectedAlarm.name = selectedAlarmName.text;
            selectedAlarm.hours = hours;
            selectedAlarm.minutes = minutes;
            selectedAlarm.daysOfWeek = alarmDaysOfWeek;
            selectedAlarm.save(); // remote save
            alarmPlayer.stop();
            selectedAlarm.enabled = true;
            showPassiveNotification(selectedAlarm.timeToRingFormated());
        } else {
            alarmModel.addAlarm(hours, minutes, alarmDaysOfWeek, selectedAlarmName.text);
            alarmPlayer.stop();
            showPassiveNotification(alarmModel.timeToRingFormated(hours, minutes, alarmDaysOfWeek));
        }
        pageStack.layers.pop();
    }
    
    actions.main: Kirigami.Action {
        iconName: "dialog-ok"
        text: i18n("Done")
        onTriggered: root.formComplete()
    }
    
    // mobile footer actions
    footer: ToolBar {
        id: toolbar
        visible: Kirigami.Settings.isMobile
        topPadding: 0; bottomPadding: 0
        rightPadding: 0; leftPadding: 0
        
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false
        
        property bool opened: false
        RowLayout {
            anchors.fill: parent
            spacing: 0
            
            Item { Layout.fillWidth: true }
            FooterToolBarButton {
                display: toolbar.opened ? AbstractButton.TextUnderIcon : AbstractButton.TextOnly
                text: i18n("Done")
                icon.name: "dialog-ok"
                onClicked: root.formComplete()
            }
            FooterToolBarButton {
                display: toolbar.opened ? AbstractButton.TextUnderIcon : AbstractButton.TextOnly
                text: i18n("Cancel")
                icon.name: "dialog-cancel"
                onClicked: appwindow.pageStack.layers.pop()
            }
            FooterToolBarButton {
                display: toolbar.opened ? AbstractButton.TextUnderIcon : AbstractButton.TextOnly
                icon.name: "view-more-symbolic"
                onClicked: toolbar.opened = !toolbar.opened
                iconSize: Kirigami.Units.iconSizes.small
                implicitWidth: Kirigami.Units.gridUnit * 2.5
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
                    implicitWidth: Kirigami.Units.gridUnit * 2.8
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
            text: i18n("Alarm Name (optional)")
            font.weight: Font.Bold
        }
        TextField {
            anchors.horizontalCenter: parent.horizontalCenter
            id: selectedAlarmName
            placeholderText: i18n("Wake Up")
            text: selectedAlarm ? selectedAlarm.name : ""
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
            width: root.width * 0.8
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
            title: i18n("Choose an audio")
            folder: shortcuts.music
            onAccepted: {
                root.ringtonePath = fileDialog.fileUrl;
                if (ringtonePath != "") {
                    if (selectedAlarm) {
                        selectedAlarm.ringtonePath = root.ringtonePath;
                        selectedAlarm.ringtoneName = root.ringtonePath.toString().split('/').pop();
                    }
                    console.log(root.ringtonePath);
                    alarmPlayer.setSource(root.ringtonePath);
                    alarmPlayer.play();
                }
                this.close();
            }
            onRejected: {
                this.close();
            }
            nameFilters: [ i18n("Audio files (*.wav *.mp3 *.ogg *.aac *.flac *.webm *.mka *.opus)"), i18n("All files (*)") ]
        }
    }
    function hoursTo12(hours){ // auxiliary function to convert 24hours to 12
        if(hours >= 12)
            return hours - 12;
        else
            return hours;
    }
}

