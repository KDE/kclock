/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.2

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.dateandtime 0.1 as DateAndTime

import "../components"
import "../components/formatUtil.js" as FormatUtil
import kclock 1.0

Kirigami.FormLayout {
    id: root
    
    property Alarm selectedAlarm: null

    // given values
    readonly property string name: selectedAlarm ? selectedAlarm.name : ""
    readonly property int hours: selectedAlarm ? selectedAlarm.hours : 0
    readonly property int minutes: selectedAlarm ? selectedAlarm.minutes : 0
    readonly property int daysOfWeek: selectedAlarm ? selectedAlarm.daysOfWeek : 0
    readonly property string audioPath: selectedAlarm ? selectedAlarm.audioPath : utilModel.getDefaultAlarmFileLocation()
    readonly property int ringDuration: selectedAlarm ? selectedAlarm.ringDuration : 5
    readonly property int snoozeDuration: selectedAlarm ? selectedAlarm.snoozeDuration : 5
    
    // values currently in form
    // the editable fields have bindings that are broken by the form
    readonly property string formName: nameField.text
    readonly property int formHours: timePicker.hours + (timePicker.pm ? 12 : 0)
    readonly property int formMinutes: timePicker.minutes
    property int formDaysOfWeek: daysOfWeek
    property string formAudioPath: audioPath
    property int formRingDuration: ringDuration
    property int formSnoozeDuration: snoozeDuration
    
    function submitForm() {
        if (selectedAlarm) { // edit existing alarm
            selectedAlarm.name = formName;
            selectedAlarm.hours = formHours;
            selectedAlarm.minutes = formMinutes;
            selectedAlarm.daysOfWeek = formDaysOfWeek;
            selectedAlarm.audioPath = formAudioPath;
            selectedAlarm.ringDuration = formRingDuration;
            selectedAlarm.snoozeDuration = formSnoozeDuration;
            selectedAlarm.enabled = true;
            showPassiveNotification(selectedAlarm.timeToRingFormatted());
        } else { // create new alarm
            alarmModel.addAlarm(formName, formHours, formMinutes, formDaysOfWeek, formAudioPath, formRingDuration, formSnoozeDuration);
            showPassiveNotification(alarmModel.timeToRingFormatted(formHours, formMinutes, formDaysOfWeek));
        }
    }
    
    wideMode: false
    
    // time picker
    DateAndTime.TimePicker {
        id: timePicker
        anchors.left: parent.left
        anchors.right: parent.right
        implicitHeight: 400
        height: 400
        width: Math.min(400, parent.width)

        hours: FormatUtil.hoursTo12(root.hours)
        minutes: root.minutes
        pm: selectedAlarm ? root.hours >= 12 : false
        
        Component.onCompleted: {
            if (!selectedAlarm) { // new alarm
                let date = new Date();
                hours = date.getHours() >= 12 ? date.getHours() - 12 : date.getHours();
                minutes = date.getMinutes();
                pm = date.getHours() >= 12;
            }
        }
    }

    // repeat day picker
    DialogComboBox {
        implicitWidth: root.width
        
        Kirigami.FormData.label: i18n("Days to repeat:")
        text: FormatUtil.getRepeatFormat(root.formDaysOfWeek)
        title: i18n("Select Days to Repeat")
        model: weekModel
        
        dialogDelegate: CheckDelegate {
            implicitWidth: Kirigami.Units.gridUnit * 16
            topPadding: Kirigami.Units.smallSpacing * 2
            bottomPadding: Kirigami.Units.smallSpacing * 2
            
            text: name
            checkState: kclockFormat.isChecked(index, root.formDaysOfWeek) ? Qt.Checked : Qt.Unchecked
            onCheckStateChanged: {
                if (checkState == Qt.Checked) {
                    root.formDaysOfWeek |= flag;
                } else {
                    root.formDaysOfWeek &= ~flag;
                }
            }
        }
    }
    
    // name field
    TextField {
        id: nameField
        Kirigami.FormData.label: i18n("Alarm Name (optional):")
        placeholderText: i18n("Wake Up")
        text: root.name
    }

    // ring duration picker
    DialogComboBox {
        id: ringDurationPicker
        implicitWidth: root.width
        
        Kirigami.FormData.label: i18n("Ring Duration:")
        text: formRingDuration === 1 ? i18n("1 minute") : i18n("%1 minutes", formRingDuration)
        title: i18n("Select Ring Duration")
        model: ListModel {
            // we can't use i18n with ListElement
            Component.onCompleted: {
                append({"name": i18n("1 minute"), "value": 1});
                append({"name": i18n("2 minutes"), "value": 2});
                append({"name": i18n("5 minutes"), "value": 5});
                append({"name": i18n("10 minutes"), "value": 10});
                append({"name": i18n("15 minutes"), "value": 15});
                append({"name": i18n("Never"), "value": -1});
            }
        }
        
        dialogDelegate: RadioDelegate {
            implicitWidth: Kirigami.Units.gridUnit * 16
            topPadding: Kirigami.Units.smallSpacing * 2
            bottomPadding: Kirigami.Units.smallSpacing * 2
            
            text: name
            checked: root.formRingDuration == value
            onCheckedChanged: {
                if (checked) {
                    root.formRingDuration = value;
                }
            }
        }
    }

    
    // snooze length picker
    DialogComboBox {
        id: snoozeLengthPicker
        implicitWidth: root.width
        
        Kirigami.FormData.label: i18n("Snooze Length:")
        title: i18n("Select Snooze Length")
        text: formSnoozeDuration === 1 ? i18n("1 minute") : i18n("%1 minutes", formSnoozeDuration)
        model: ListModel {
            // we can't use i18n with ListElement
            Component.onCompleted: {
                append({"name": i18n("1 minute"), "value": 1});
                append({"name": i18n("2 minutes"), "value": 2});
                append({"name": i18n("5 minutes"), "value": 5});
                append({"name": i18n("10 minutes"), "value": 10});
                append({"name": i18n("15 minutes"), "value": 15});
                append({"name": i18n("30 minutes"), "value": 30});
                append({"name": i18n("1 hour"), "value": 60});
            }
        }
        
        dialogDelegate: RadioDelegate {
            implicitWidth: Kirigami.Units.gridUnit * 16
            topPadding: Kirigami.Units.smallSpacing * 2
            bottomPadding: Kirigami.Units.smallSpacing * 2
            
            text: name
            checked: root.formSnoozeDuration == value
            onCheckedChanged: {
                if (checked) {
                    root.formSnoozeDuration = value;
                }
            }
        }
    }
    
    // audio path field
    Button {
        id: audioPathField
        implicitWidth: root.width
        width: root.width
        
        Kirigami.FormData.label: i18n("Ring Sound:")
        
        onClicked: applicationWindow().pageStack.push(Qt.resolvedUrl("SoundPickerPage.qml"), { alarmForm: root });
        
        text: {
            if (root.formAudioPath == utilModel.getDefaultAlarmFileLocation())  {
                return i18n("Default Sound");
            } else {
                let split = root.formAudioPath.split('/')
                return split[split.length - 1];
            }
        }
        
        Kirigami.Icon {
            source: "go-down-symbolic"
            implicitWidth: Kirigami.Units.iconSizes.small
            implicitHeight: Kirigami.Units.iconSizes.small
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: Kirigami.Units.smallSpacing
        }
    }
}
