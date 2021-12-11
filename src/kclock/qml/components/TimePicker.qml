/*
 * Copyright 2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.2

import org.kde.kirigami 2.15 as Kirigami

RowLayout {
    id: root
    spacing: Kirigami.Units.smallSpacing
    
    property int hours: 0
    property int minutes: 0
    property bool twelveHourTime: true // am/pm
    
    onHoursChanged: hoursSpinbox.value = twelveHourTime ? (hours + 1) % 12 : hours
    onMinutesChanged: minutesSpinbox.value = minutes
    
    // note: for 12-hour time, we have hours from 1-12 (0'o clock displays as 12)
    //       for 24-hour time, we have hours from 0-23
    TimePickerSpinBox {
        id: hoursSpinbox
        value: twelveHourTime ? (hours + 1) % 12 : hours
        from: 1
        to: twelveHourTime ? 12 : 23
        
        property bool manualUpdate: false
        onValueChanged: {
            if (!manualUpdate) {
                manualUpdate = true;
                if (root.twelveHourTime) {
                    root.hours = amPmToggle.checked ? (((hours % 12) + 12) % 12) : (hours % 12)
                } else {
                    root.hours = hours;
                }
            }
            manualUpdate = false;
        }
    }
    
    Label {
        text: ":"
    }
    
    TimePickerSpinBox {
        id: minutesSpinbox
        value: minutes
        from: 0
        to: 59
        
        property bool manualUpdate: false
        onValueChanged: {
            if (!manualUpdate) {
                manualUpdate = true;
                root.minutes = minutes;
            }
            manualUpdate = false;
        }
    }
    
    Button {
        id: amPmToggle
        visible: twelveHourTime
        checkable: true
        text: i18n(hours < 12 ? i18n("AM") : i18n("PM"))
        Layout.fillHeight: true
    }
}
