/*
 * Copyright 2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

Button {
    id: root
    required property var hoursSpinBox
    required property var minutesSpinBox
    required property var secondsSpinBox
    
    property int minutes: 0
    property int hours: 0
    
    implicitWidth: Kirigami.Units.gridUnit * 4
    text: i18n("1 m")
    
    checked: shouldBeChecked()
    
    function shouldBeChecked() {
        return hoursSpinBox.value === hours && minutesSpinBox.value === minutes && secondsSpinBox.value === 0;
    }
    
    Connections {
        target: hoursSpinBox
        function onValueChanged() {
            root.checked = root.shouldBeChecked();
        }
    }
    Connections {
        target: minutesSpinBox
        function onValueChanged() {
            root.checked = root.shouldBeChecked();
        }
    }
    Connections {
        target: secondsSpinBox
        function onValueChanged() {
            root.checked = root.shouldBeChecked();
        }
    }
    
    onClicked: {
        spinBoxHours.value = hours;
        spinBoxMinutes.value = minutes;
        spinBoxSeconds.value = 0;
        focus = false;
    }
}
