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

Item {
    implicitHeight: timeLabels.implicitHeight + lapText.implicitHeight

    Column {
        anchors.horizontalCenter: parent.horizontalCenter
        y: Math.round((parent.height - height) / 2)
        Behavior on y {
            NumberAnimation { duration: Kirigami.Units.longDuration; easing.type: Easing.InOutQuad }
        }

        // toggle when clicked
        MouseArea {
            id: stopwatchArea
            anchors.horizontalCenter: parent.horizontalCenter
            width: timeLabels.implicitWidth
            height: timeLabels.implicitHeight
            onClicked: KClock.StopwatchTimer.toggle()

            Row {
                id: timeLabels
                spacing: Math.round(Kirigami.Units.smallSpacing / 2)

                QQC.Label {
                    id: text
                    text: KClock.StopwatchTimer.display + "."
                    textFormat: Text.PlainText
                    font.features: { "tnum": 1 }
                    font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 4.75)
                    font.weight: Font.ExtraLight
                }
                QQC.Label {
                    id: secondsText
                    anchors.baseline: text.baseline
                    text: KClock.StopwatchTimer.small
                    textFormat: Text.PlainText
                    font.features: { "tnum": 1 }
                    font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 3.75)
                    font.weight: Font.ExtraLight
                }
            }
        }

        // elapsed duration for a lap
        QQC.Label {
            id: lapText
            anchors.horizontalCenter: parent.horizontalCenter
            visible: KClock.StopwatchModel.mostRecentLapTime > 0
            opacity: 0.8

            font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 1.1)
            font.weight: Font.Bold

            text: {
                const duration = KClock.StopwatchTimer.elapsedTime - KClock.StopwatchModel.mostRecentLapTime;

                const hours = KClock.UtilModel.displayTwoDigits(KClock.UtilModel.msToHoursPart(duration));
                const minutes = KClock.UtilModel.displayTwoDigits(KClock.UtilModel.msToMinutesPart(duration));
                const seconds = KClock.UtilModel.displayTwoDigits(KClock.UtilModel.msToSecondsPart(duration));
                const small = KClock.UtilModel.displayTwoDigits(KClock.UtilModel.msToSmallPart(duration));

                // only show hours if we have passed an hour
                if (hours === '00') {
                    return "%1:%2.%3".arg(minutes).arg(seconds).arg(small);
                } else {
                    return "%1:%2:%3.%4".arg(hours).arg(minutes).arg(seconds).arg(small);
                }
            }
            textFormat: Text.PlainText
        }
    }
}