/*
    SPDX-FileCopyrightText: 2020 HanY <hanyoung@protonmail.com>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick 2.1
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.kirigami 2.11 as Kirigami

Item {
    Plasmoid.backgroundHints: "ShadowBackground";
    Plasmoid.fullRepresentation: Item {
        Layout.preferredWidth: mainLayout.width
        Layout.preferredHeight: mainLayout.height
        MouseArea {
            anchors.fill: parent
            onClicked: plasmoid.nativeInterface.openKClock()
        }
        ColumnLayout {
            id: mainLayout
            spacing: 0
            PlasmaComponents.Label {
                text: plasmoid.nativeInterface.time
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 6
                color: "white"
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                PlasmaComponents.Label {
                    text: plasmoid.nativeInterface.date
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
                    color: "white"
                    Layout.rightMargin: Kirigami.Units.gridUnit
                }
                RowLayout {
                    id: alarmLabel
                    visible: plasmoid.nativeInterface.hasAlarm
                    Kirigami.Icon {
                        source: "notifications"
                        Layout.preferredHeight: alarmTime.height
                        Layout.preferredWidth: alarmTime.height
                    }
                    PlasmaComponents.Label {
                        id: alarmTime
                        Layout.alignment: Qt.AlignCenter
                        text: plasmoid.nativeInterface.alarmTime
                        color: "white"
                        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
                    }
                }
            }
            RowLayout {
                id: weatherLayout

                // left city name and weather description
                ColumnLayout {
                    PlasmaComponents.Label {
                        text: plasmoid.nativeInterface.cityName
                        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 2
                        font.bold: true
                        color: "white"
                    }
                    PlasmaComponents.Label {
                        text: plasmoid.nativeInterface.description
                        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
                        color: "white"
                    }
                }
                // center weather icon
                Kirigami.Icon {
                    source: plasmoid.nativeInterface.weatherIcon
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 4
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 4
                }
                // right temperature
                ColumnLayout {
                    PlasmaComponents.Label {
                        text: plasmoid.nativeInterface.tempNow
                        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 2
                        font.bold: true
                        color: "white"
                    }
                    RowLayout {
                        PlasmaComponents.Label {
                            text: plasmoid.nativeInterface.maxTemp
                            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
                            font.bold: true
                            color: "white"
                        }
                        PlasmaComponents.Label {
                            Layout.alignment: Qt.AlignBottom
                            text: plasmoid.nativeInterface.minTemp
                            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1
                            font.bold: true
                            color: "white"
                        }
                    }
                }
            }
        }
    }
//    Plasmoid.compactRepresentation: Kirigami.Icon {
//        source: "notifications"
//        height: Kirigami.Units.gridUnit
//        width: Kirigami.Units.gridUnit
//    }
}
