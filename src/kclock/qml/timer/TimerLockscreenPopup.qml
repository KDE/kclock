/*
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
    objectName: "TimerLockscreenPopup"

    property string timerPopupId
    property Timer timer

    title: i18n("Timer is ringing")
    visibility: Window.FullScreen
    color: Kirigami.Theme.backgroundColor
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    Connections {
        target: TimerModel
        function onStatusChanged(status) {
            timer = TimerModel.timerByUuid(timerPopupId);
        }
    }

    LockscreenAlertView {
        anchors.fill: parent

        formattedTime: timer ? timer.lengthPretty : ""
        name: timer ? (timer.name ? timer.name : i18n("Timer")) : i18n("Timer not found")

        mainButtonName: i18n("Stop")
        secondaryButtonName: i18n("Repeat")

        // dismiss timer
        onMainButtonPressed: {
            if (root.timer) {
                root.timer.dismiss();
            }
            root.close();
        }

        // repeat timer
        onSecondaryButtonPressed: {
            if (root.timer) {
                root.timer.dismiss();
                root.timer.reset();
                root.timer.toggleRunning();
            }
            root.close();
        }
    }
}
