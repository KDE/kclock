/*
 * Copyright 2021 Devin Lin <espidev@gmail.com>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import kclock

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
                onActiveFocusChanged: {
                    if (activeFocus) {
                        (contentItem as TextInput)?.selectAll();
                    }
                }
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
                onActiveFocusChanged: {
                    if (activeFocus) {
                        (contentItem as TextInput)?.selectAll();
                    }
                }
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
                onActiveFocusChanged: {
                    if (activeFocus) {
                        (contentItem as TextInput)?.selectAll();
                    }
                }
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
}
