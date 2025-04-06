/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.dateandtime as DateAndTime

import org.kde.kclock

Kirigami.FormLayout {
    id: root

    property Alarm selectedAlarm: null

    // given values
    readonly property string name: selectedAlarm ? selectedAlarm.name : ""
    readonly property int hours: selectedAlarm ? selectedAlarm.hours : 0
    readonly property int minutes: selectedAlarm ? selectedAlarm.minutes : 0
    readonly property int daysOfWeek: selectedAlarm ? selectedAlarm.daysOfWeek : 0
    readonly property string audioPath: selectedAlarm ? selectedAlarm.audioPath : UtilModel.getDefaultAlarmFileLocation()
    readonly property int ringDuration: selectedAlarm ? selectedAlarm.ringDuration : 5
    readonly property int snoozeDuration: selectedAlarm ? selectedAlarm.snoozeDuration : 5

    // values currently in form
    // the editable fields have bindings that are broken by the form
    readonly property string formName: nameField.text
    readonly property int formHours: timePicker.hours
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
            AlarmModel.addAlarm(formName, formHours, formMinutes, formDaysOfWeek, formAudioPath, formRingDuration, formSnoozeDuration);
            showPassiveNotification(AlarmModel.timeToRingFormatted(formHours, formMinutes, formDaysOfWeek));
        }
    }

    wideMode: false

    // time picker
    TimePicker {
        id: timePicker
        anchors.left: parent.left
        anchors.right: parent.right
        width: Math.min(400, parent.width)

        hours: root.hours
        minutes: root.minutes

        Component.onCompleted: {
            if (!selectedAlarm) { // new alarm
                let date = new Date();
                hours = date.getHours();
                minutes = date.getMinutes();
            }
        }
    }

    // repeat day picker
    DialogComboBox {
        implicitWidth: root.width

        Kirigami.FormData.label: i18n("Days to repeat:")
        text: UtilModel.repeatFormat(root.formDaysOfWeek)
        title: i18n("Select Days to Repeat")
        model: WeekModel

        dialogDelegate: CheckDelegate {
            implicitWidth: Kirigami.Units.gridUnit * 16
            topPadding: Kirigami.Units.smallSpacing * 2
            bottomPadding: Kirigami.Units.smallSpacing * 2

            text: name
            checkState: KClockFormat.isChecked(index, root.formDaysOfWeek) ? Qt.Checked : Qt.Unchecked
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
        text: {
            if (formRingDuration === -1) {
                return i18n("None");
            } else if (formRingDuration === 1) {
                return i18n("1 minute");
            } else {
                return i18n("%1 minutes", formRingDuration);
            }
        }
        title: i18n("Select Ring Duration")
        model: [
            {"name": i18n("None"), "value": -1},
            {"name": i18n("1 minute"), "value": 1},
            {"name": i18n("2 minutes"), "value": 2},
            {"name": i18n("5 minutes"), "value": 5},
            {"name": i18n("10 minutes"), "value": 10},
            {"name": i18n("15 minutes"), "value": 15},
            {"name": i18n("30 minutes"), "value": 30},
            {"name": i18n("1 hour"), "value": 60}
        ]

        dialogDelegate: RadioDelegate {
            implicitWidth: Kirigami.Units.gridUnit * 16
            topPadding: Kirigami.Units.smallSpacing * 2
            bottomPadding: Kirigami.Units.smallSpacing * 2

            text: modelData.name
            checked: root.formRingDuration == modelData.value
            onCheckedChanged: {
                if (checked) {
                    root.formRingDuration = modelData.value;
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
        model: [
            {"name": i18n("1 minute"), "value": 1},
            {"name": i18n("2 minutes"), "value": 2},
            {"name": i18n("5 minutes"), "value": 5},
            {"name": i18n("10 minutes"), "value": 10},
            {"name": i18n("15 minutes"), "value": 15},
            {"name": i18n("30 minutes"), "value": 30},
            {"name": i18n("1 hour"), "value": 60},
        ]

        dialogDelegate: RadioDelegate {
            implicitWidth: Kirigami.Units.gridUnit * 16
            topPadding: Kirigami.Units.smallSpacing * 2
            bottomPadding: Kirigami.Units.smallSpacing * 2

            text: modelData.name
            checked: root.formSnoozeDuration == modelData.value
            onCheckedChanged: {
                if (checked) {
                    root.formSnoozeDuration = modelData.value;
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
            if (root.formAudioPath == UtilModel.getDefaultAlarmFileLocation())  {
                return i18n("Default Sound");
            } else {
                let split = root.formAudioPath.split('/')
                return split[split.length - 1].split('.')[0];
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
