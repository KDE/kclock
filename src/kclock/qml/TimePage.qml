/*
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *           2020 Devin Lin <espidev@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import QtQuick.Shapes 1.12
import org.kde.kirigami 2.12 as Kirigami

Kirigami.ScrollablePage {
    id: timePage
    
    property int clockRadius: Kirigami.Units.gridUnit * 4

    title: i18n("Time")
    icon.name: "clock"

    mainAction: Kirigami.Action {
        iconName: "globe"
        text: i18n("Edit")
        onTriggered: {
            timeZoneSelectorModel.update();
            timeZoneSelect.active = true;
            timeZoneSelect.item.open();
        }
    }
    
    TimeZoneSelectWrapper {
        id: timeZoneSelect
        active: false
    }
    
    header: RowLayout {
        id: bigTimeDisplay
        height: clockItemLoader.height + Kirigami.Units.gridUnit * 0.5
        implicitHeight: height
        anchors.left: parent.left
        anchors.right: parent.right
        
        // left side - analog clock
        Loader {
            id: clockItemLoader
            Layout.alignment: Qt.AlignHCenter
            width: Math.round(clockRadius * 2 + Kirigami.Units.gridUnit * 0.5)
            height: clockRadius * 2 + Kirigami.Units.gridUnit
            
            asynchronous: true
            
            sourceComponent: Item {
                id: clockItem
                AnalogClock {
                    id: analogClock
                    anchors.centerIn: parent
                    height: clockRadius * 2 
                    width: clockRadius * 2
                }
            }
        }
        
        // right side - digital clock + location
        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter
            Layout.rightMargin: Kirigami.Units.smallSpacing
            Label {
                Layout.alignment: Qt.AlignRight
                font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 2.8)
                font.family: clockFont.name
                font.weight: Font.Light
                text: kclockFormat.currentTime
            }
            Label {
                Layout.alignment: Qt.AlignRight
                font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 1.2)
                text: utilModel.tzName
                color: Kirigami.Theme.textColor
            }
        }
    }
    
    // time zones
    ListView {
        model: timeZoneShowModel
        id: zoneList
        currentIndex: -1 // no default selection
        
        reuseItems: true
        
        add: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: Kirigami.Units.shortDuration }
        }
        remove: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: Kirigami.Units.shortDuration }
        }
        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: Kirigami.Units.longDuration; easing.type: Easing.InOutQuad}
        }
        
        // no timezones placeholder
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: zoneList.count == 0
            text: i18n("Add timezone")
        }
        
        delegate: TimeZoneEntry {
            tzId: model.id
            tzRelative: model.relativeTime
            tzTime: model.timeString
        }
    }
}
