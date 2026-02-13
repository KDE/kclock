/*
 * Copyright 2025 Tushar Gupta <tushar.197712@gmail.com>
 * Copyright 2026 Micah Stanley <stanleymicah@proton.me>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kclock

Window {
    id: root
    objectName: "AlarmLockscreenPopup"

    property string alarmPopupId
    property Alarm alarm

    title: i18n("Alarm is ringing")
    visibility: Window.FullScreen
    color: Kirigami.Theme.backgroundColor
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    Connections {
        target: AlarmModel
        function onStatusChanged(status) {
            alarm = AlarmModel.alarmByUuid(alarmPopupId);
        }
    }

    LockscreenAlertView {
        anchors.fill: parent

        formattedTime: alarm ? alarm.formattedTime : ""
        name: alarm ? (alarm.name ? alarm.name : i18n("Alarm")) : i18n("Alarm not found")

        mainButtonName: i18n("Snooze")
        secondaryButtonName: i18n("Dismiss")

        // snooze alarm
        onMainButtonPressed: {
            if (root.alarm) {
                root.alarm.snooze();
            }
            root.close();
        }

        // dismiss alarm
        onSecondaryButtonPressed: {
            if (root.alarm) {
                root.alarm.dismiss();
            }
            root.close();
        }
    }
}
