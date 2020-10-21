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

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import QtQuick.Shapes 1.12
import org.kde.kirigami 2.12 as Kirigami

Kirigami.ScrollablePage {
    
    title: "Time"
    icon.name: "clock"
    // time zones
    ListView {
        model: timeZoneShowModel
        id: zoneList
        
        // no timezones placeholder
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: Kirigami.Units.largeSpacing
            visible: zoneList.count == 0
            text: i18n("Add timezone")
        }
        
        // analog clock header
        headerPositioning: ListView.InlineHeader
        header: RowLayout {
            id: bigTimeDisplay
            height: clockItem.height + Kirigami.Units.gridUnit
            anchors.left: parent.left
            anchors.right: parent.right
            
            // left side - analog clock
            Item {
                id: clockItem
                Layout.alignment: Qt.AlignHCenter
                width: analogClock.clockRadius * 2 + Kirigami.Units.gridUnit * 0.5
                height: analogClock.clockRadius * 2 + Kirigami.Units.gridUnit
                AnalogClock {
                    id: analogClock
                    clockRadius: Kirigami.Units.gridUnit * 4
                }
            }
            
            // right side - digital clock + location
            ColumnLayout {
                Layout.alignment: Qt.AlignHCenter
                Label {
                    Layout.alignment: Qt.AlignRight
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 3.0
                    font.family: clockFont.name
                    color: Kirigami.Theme.highlightColor
                    text: kclockFormat.currentTime
                }
                Label {
                    Layout.alignment: Qt.AlignRight
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.2
                    text: utilModel.tzName
                    color: Kirigami.Theme.textColor
                }
            }
        }
        
        ScrollBar.vertical: ScrollBar {}
        
        delegate: timeZoneDelegate
    }
    
    // time zone entry
    Component {
        id: timeZoneDelegate
        TimeZoneEntry {
            tzId: model.id
            tzRelative: model.relativeTime
            tzTime: model.timeString
        }
    }
    
    //globalToolBarStyle: Kirigami.ApplicationHeaderStyle.ToolBar
    mainAction: Kirigami.Action {
        iconName: "globe"
        text: i18n("Edit")
        onTriggered: {
            pageStack.layers.push("qrc:/qml/TimeZoneSelectPage.qml")
        }
    }
}
