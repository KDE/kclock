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

Item {
    id: root

    property string formattedTime: ""
    property string name: ""

    property string mainButtonName: ""
    property string secondaryButtonName: ""

    signal mainButtonPressed()
    signal secondaryButtonPressed()

    ColumnLayout {
        anchors.centerIn: parent
        spacing: Kirigami.Units.largeSpacing * 2
        width: root.width

        Kirigami.Icon {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredHeight: Kirigami.Units.gridUnit * 4
            Layout.preferredWidth: Layout.preferredHeight
            source: "kalarm-symbolic"

            SequentialAnimation on rotation {
                id: ringAnimation
                running: true
                loops: Animation.Infinite

                RotationAnimation {
                    to: 6
                    duration: 100
                    easing.type: Easing.InOutQuad
                }

                RotationAnimation {
                    to: -6
                    duration: 200
                    easing.type: Easing.InOutQuad
                }

                RotationAnimation {
                    to: 0
                    duration: 100
                    easing.type: Easing.InOutQuad
                }
            }
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            font.weight: Font.Light
            font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 5)
            text: root.formattedTime ? root.formattedTime : "00:00"
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 1.5)
            text: root.name
            wrapMode: Text.Wrap
        }

        Item {
            Layout.preferredHeight: Kirigami.Units.gridUnit * 5
        }
    }

    ColumnLayout {
        height: Kirigami.Units.gridUnit * 7
        width: root.width
        anchors.bottom: parent.bottom

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: Kirigami.Units.largeSpacing
            Layout.maximumWidth: Math.min(root.width, Kirigami.Units.gridUnit * 24)
            uniformCellSizes: true
            Layout.margins: Kirigami.Units.largeSpacing * 3

            Button {
                id: snoozeButton
                Layout.preferredHeight: Math.round(Kirigami.Units.gridUnit * 2.5)
                Layout.fillWidth: true
                focusPolicy: Qt.StrongFocus

                text: mainButtonName

                onClicked: {
                    root.mainButtonPressed();
                }
            }

            Button {
                id: stopButton
                Layout.preferredHeight: Math.round(Kirigami.Units.gridUnit * 2.5)
                Layout.fillWidth: true
                focusPolicy: Qt.StrongFocus

                text: secondaryButtonName

                onClicked: {
                    root.secondaryButtonPressed();
                }

            }
        }
    }
}
