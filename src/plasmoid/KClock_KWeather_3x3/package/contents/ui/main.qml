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
        property int fontSize: mainItem.height / 4
        id: mainItem
        Layout.preferredHeight: Kirigami.Units.gridUnit * 12
        Layout.preferredWidth: fontSize * 24
        ColumnLayout {
            id: mainLayout
            Layout.fillWidth: true
            spacing: 0
            PlasmaComponents.Label {
                text: plasmoid.nativeInterface.time
                font.pointSize: fontSize
                color: "white"
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }
            RowLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                PlasmaComponents.Label {
                    text: plasmoid.nativeInterface.date
                    font.pointSize: fontSize / 3
                    color: "white"
                    Layout.rightMargin: Kirigami.Units.gridUnit
                }
                RowLayout {
                    id: alarmLabel
                    Layout.fillWidth: true
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
                        font.pointSize: fontSize / 3
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: plasmoid.nativeInterface.openKClock()
                }
            }
            RowLayout {
                id: weatherLayout
                Layout.fillWidth: true
                // left city name and weather description
                ColumnLayout {
                    Layout.fillWidth: true
                    PlasmaComponents.Label {
                        id: cityName
                        text: plasmoid.nativeInterface.cityName
                        font.pointSize: fontSize / 2
                        font.bold: true
                        color: "white"
                    }
                    PlasmaComponents.Label {
                        text: plasmoid.nativeInterface.description
                        font.pointSize: fontSize / 3
                        color: "white"
                    }
                }
                // center weather icon
                Kirigami.Icon {
                    source: plasmoid.nativeInterface.weatherIcon
                    Layout.preferredHeight: fontSize
                    Layout.preferredWidth: fontSize
                }
                // right temperature
                ColumnLayout {
                    Layout.fillWidth: true
                    PlasmaComponents.Label {
                        id: tempNow
                        text: plasmoid.nativeInterface.tempNow
                        font.pointSize: fontSize / 2
                        font.bold: true
                        color: "white"
                    }
                    RowLayout {
                        PlasmaComponents.Label {
                            text: plasmoid.nativeInterface.maxTemp
                            font.pointSize: fontSize / 3
                            font.bold: true
                            color: "white"
                        }
                        PlasmaComponents.Label {
                            Layout.alignment: Qt.AlignBottom
                            text: plasmoid.nativeInterface.minTemp
                            font.pointSize: fontSize / 4
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
