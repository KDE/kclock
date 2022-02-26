/*
 * Copyright 2021 Devin Lin <espidev@gmail.com>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2

import org.kde.kirigami 2.15 as Kirigami

import kclock 1.0

Kirigami.FormLayout {
    id: form

    property alias name: label.text
    property alias commandTimeout: commandTimeoutField.text

    function setDuration(duration) {
        hours = duration / 60 / 60;
        minutes = duration % (60 * 60) / 60;
        seconds = duration % 60;
    }
    function getDuration() {
        return spinBoxHours.value * 60 * 60 + spinBoxMinutes.value * 60 + spinBoxSeconds.value;
    }

    ColumnLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: Kirigami.Units.smallSpacing

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: Kirigami.Units.smallSpacing
            PresetDurationButton {
                text: i18n("1 m")
                hours: 0
                minutes: 1
                hoursSpinBox: spinBoxHours 
                minutesSpinBox: spinBoxMinutes
                secondsSpinBox: spinBoxSeconds
            }
            PresetDurationButton {
                text: i18n("5 m")
                hours: 0
                minutes: 5
                hoursSpinBox: spinBoxHours 
                minutesSpinBox: spinBoxMinutes
                secondsSpinBox: spinBoxSeconds
            }
            PresetDurationButton {
                text: i18n("10 m")
                hours: 0
                minutes: 10
                hoursSpinBox: spinBoxHours 
                minutesSpinBox: spinBoxMinutes
                secondsSpinBox: spinBoxSeconds
            }
        }
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: Kirigami.Units.smallSpacing
            PresetDurationButton {
                text: i18n("15 m")
                hours: 0
                minutes: 15
                hoursSpinBox: spinBoxHours 
                minutesSpinBox: spinBoxMinutes
                secondsSpinBox: spinBoxSeconds
            }
            PresetDurationButton {
                text: i18n("30 m")
                hours: 0
                minutes: 30
                hoursSpinBox: spinBoxHours 
                minutesSpinBox: spinBoxMinutes
                secondsSpinBox: spinBoxSeconds
            }
            PresetDurationButton {
                text: i18n("1 h")
                hours: 1
                minutes: 0
                hoursSpinBox: spinBoxHours 
                minutesSpinBox: spinBoxMinutes
                secondsSpinBox: spinBoxSeconds
            }
        }
    }
    Kirigami.Separator {}

    ColumnLayout {
        Kirigami.FormData.label: i18n("<b>Duration:</b>")
        Kirigami.FormData.buddyFor: spinBoxHours
        RowLayout {
            SpinBox {
                id: spinBoxHours
                value: 0 // default
                Layout.preferredWidth: Kirigami.Units.gridUnit * 6
            }
            Label {
                text: i18n("hours")
            }
        }
        RowLayout {
            SpinBox {
                id: spinBoxMinutes
                to: 59
                value: 5 // default
                Layout.preferredWidth: Kirigami.Units.gridUnit * 6
            }
            Label {
                text: i18n("minutes")
            }
        }
        RowLayout {
            SpinBox {
                id: spinBoxSeconds
                to: 59
                value: 0 // default
                Layout.preferredWidth: Kirigami.Units.gridUnit * 6
            }
            Label {
                text: i18n("seconds")
            }
        }
    }
    TextField {
        id: label
        Kirigami.FormData.label: i18n("<b>Label:</b>")
        text: i18n("Timer") // default
        focus: true
    }
    TextField {
        id: commandTimeoutField
        Kirigami.FormData.label: i18n("<b>Command at timeout:</b>")
        font.family: "Monospace"
        focus: true
        placeholderText: i18n("optional")
    }
    RowLayout {
        Layout.alignment: Qt.AlignHCenter
        spacing: Kirigami.Units.smallSpacing
        visible: repeater.count > 0
        Connections {
            target: repeater
            function onCountChanged() {
                // open presets every time a preset is added
                showPresets = true;
            }
        }
        
        Button {
            id: presetButton
            text: showPresets ? i18n("Hide Presets") : i18n("Show Presets")
            onClicked: showPresets = !showPresets
        }
        ToolButton {
            icon.name: "delete"
            text: i18n("Toggle Delete")
            onClicked: showDelete = !showDelete
            visible: showPresets
            checkable: true
            checked: false
        }
    }
    Flow {
        spacing: Kirigami.Units.smallSpacing
        visible: showPresets && Kirigami.Settings.isMobile && repeater.count > 0
        Layout.fillWidth: true

        Repeater {
            id: repeater
            model: TimerPresetModel

            Button {
                text: showDelete ? "Delete" : preset.presetName
                onClicked: showDelete ? TimerPresetModel.deletePreset(index) : loader.createTimer(timerForm.getDuration(), timerForm.name, timerForm.commandTimeout) & close();

            }
        }
    }
}
