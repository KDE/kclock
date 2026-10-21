/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 * Copyright 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.kclock as KClock

MouseArea {
    id: pipArea

    readonly property bool running: !KClock.StopwatchTimer.stopped && !KClock.StopwatchTimer.paused

    objectName: "StopwatchPip"
    onClicked: KClock.StopwatchTimer.toggle()

    KClock.ClockDisplay {
        width: parent.width
        anchors.verticalCenter: parent.verticalCenter
    }

    RowLayout {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: Kirigami.Units.largeSpacing
        }
        opacity: KClock.PipHandler.hovered ? 1 : 0
        visible: opacity > 0

        Behavior on opacity {
            NumberAnimation {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }

        QQC.ToolButton {
            Layout.alignment: Qt.AlignHCenter
            text: {
                if (pipArea.running) {
                    return i18nc("@action:button", "Pause");
                } else if (KClock.StopwatchTimer.paused) {
                    return i18nc("@action:button", "Resume");
                } else {
                    return i18nc("@action:button", "Start");
                }
            }
            icon.name: pipArea.running ? "chronometer-pause" : "chronometer-start"
            onClicked: KClock.StopwatchTimer.toggle()
        }

        QQC.ToolButton {
            Layout.alignment: Qt.AlignHCenter
            icon.name: pipArea.running ? "chronometer-lap" : "chronometer-reset"
            text: pipArea.running ? i18nc("@action:button", "Lap") : i18nc("@action:button", "Reset")
            onClicked: {
                if (pipArea.running) {
                    KClock.StopwatchModel.addLap();
                } else {
                    KClock.StopwatchTimer.reset();
                }
            }
        }
    }
}