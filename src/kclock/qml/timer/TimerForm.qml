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

import org.kde.kclock

Kirigami.FormLayout {
    id: form

    property alias name: label.text
    property alias looping: loopingCheckBox.checked
    property alias commandTimeout: commandTimeoutField.text

    function setDuration(duration) {
        spinBoxHours.value = duration / 60 / 60;
        spinBoxMinutes.value = duration % (60 * 60) / 60;
        spinBoxSeconds.value = duration % 60;
    }
    function getDuration() {
        return spinBoxHours.value * 60 * 60 + spinBoxMinutes.value * 60 + spinBoxSeconds.value;
    }

    GridLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        rowSpacing: Kirigami.Units.smallSpacing
        columnSpacing: Kirigami.Units.smallSpacing
        columns: 3

        Repeater {
            model: [
                {label: i18n("1 min"), h: 0, m: 1},
                {label: i18n("5 min"), h: 0, m: 5},
                {label: i18n("10 min"), h: 0, m: 10},
                {label: i18n("15 min"), h: 0, m: 15},
                {label: i18n("30 min"), h: 0, m: 30},
                {label: i18n("1 h"), h: 1, m: 0},
            ]

            PresetDurationButton {
                required property string label
                required property int h
                required property int m
                text: label
                hours: h
                minutes: m
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

    CheckBox {
        id: loopingCheckBox
        text: i18n("Loop Timer")
        icon.name: "media-repeat-all"
    }

    TextField {
        id: label
        Kirigami.FormData.label: i18n("<b>Label:</b>")
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
