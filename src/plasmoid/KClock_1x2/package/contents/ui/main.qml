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
        Layout.preferredHeight: plasmoid.nativeInterface.hasAlarm ? Kirigami.Theme.defaultFont.pointSize * 8 : Kirigami.Theme.defaultFont.pointSize * 16
        Layout.preferredWidth: Kirigami.Settings.isMobile ? plasmoid.screenGeometry.width : Kirigami.Units.gridUnit * 20
        Layout.alignment: Qt.AlignHCenter
        
        MouseArea {
            anchors.fill: parent
            onClicked: plasmoid.nativeInterface.openKClock()
        }
        
        ColumnLayout {
            id: mainDisplay
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 0
            
            PlasmaComponents.Label {
                text: Qt.formatTime(timeSource.data["Local"]["DateTime"], plasmoid.configuration.twelveHourTime ? "h:mm ap" : "h:mm")
                font.pointSize: fontSize
                font.weight: Font.ExtraLight
                color: "white"
                Layout.alignment: Qt.AlignHCenter
            }
            
            PlasmaComponents.Label {
                text: plasmoid.nativeInterface.date
                font.pointSize: Math.round(fontSize / 4)
                color: "white"
                Layout.alignment: Qt.AlignHCenter
                visible: plasmoid.configuration.showDate
            }
            
            RowLayout {
                Layout.topMargin: PlasmaCore.Units.smallSpacing
                Layout.alignment: Qt.AlignHCenter
                
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
