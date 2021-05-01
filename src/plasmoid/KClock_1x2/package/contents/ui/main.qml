/*
    SPDX-FileCopyrightText: 2020 HanY <hanyoung@protonmail.com>
    SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick 2.1
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.kirigami 2.11 as Kirigami

Item {
    Plasmoid.status: plasmoid.nativeInterface.hasAlarm ? PlasmaCore.Types.ActiveStatus : PlasmaCore.Types.Hidden
    Plasmoid.backgroundHints: "ShadowBackground";
    
    PlasmaCore.DataSource {
        id: timeSource
        engine: "time"
        connectedSources: ["Local"]
        interval: 1000
    }
    
    Plasmoid.fullRepresentation: Item {
        property int fontSize: Math.round(mainItem.width / 5)
        
        id: mainItem
        Layout.preferredHeight: Kirigami.Theme.defaultFont.pointSize * 10
        Layout.preferredWidth: Kirigami.Settings.isMobile ? Math.round(plasmoid.screenGeometry.width * 0.5) : Kirigami.Units.gridUnit * 12
        Layout.alignment: Qt.AlignHCenter
        
        property var layoutAlignment: plasmoid.configuration.textAlignment == "Left" ? Qt.AlignLeft :
                                        plasmoid.configuration.textAlignment == "Right" ? Qt.AlignRight : Qt.AlignHCenter
        
        MouseArea {
            anchors.fill: parent
            onClicked: plasmoid.nativeInterface.openKClock()
        }
        
        ColumnLayout {
            id: mainDisplay
            anchors.centerIn: parent
            spacing: 0
            
            PlasmaComponents.Label {
                text: Qt.formatTime(timeSource.data["Local"]["DateTime"], plasmoid.configuration.twelveHourTime ? "h:mm ap" : "h:mm")
                font.pointSize: fontSize
                font.weight: Font.ExtraLight
                color: "white"
                Layout.alignment: mainItem.layoutAlignment
            }
            
            PlasmaComponents.Label {
                text: plasmoid.nativeInterface.date
                font.pointSize: Math.round(fontSize / 4)
                color: "white"
                Layout.alignment: mainItem.layoutAlignment
                visible: plasmoid.configuration.showDate
            }
            
            RowLayout {
                Layout.topMargin: PlasmaCore.Units.smallSpacing
                Layout.alignment: mainItem.layoutAlignment
                
                Kirigami.Icon {
                    visible: plasmoid.nativeInterface.hasAlarm && plasmoid.configuration.showAlarms
                    source: "notifications"
                    Layout.preferredHeight: Math.round(alarmTime.height)
                    Layout.preferredWidth: Math.round(alarmTime.height)
                }
                PlasmaComponents.Label {
                    visible: plasmoid.nativeInterface.hasAlarm && plasmoid.configuration.showAlarms
                    id: alarmTime
                    text: plasmoid.nativeInterface.alarmTime
                    color: "white"
                    font.pointSize: Math.round(fontSize / 4)
                }
            }
        }
    }
}
