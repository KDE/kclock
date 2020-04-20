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
import org.kde.kirigami 2.4 as Kirigami

Kirigami.Page {
    
    title: "Time"
    
    property date currentDate: new Date()
    
    RowLayout {
        id: bigTimeDisplay
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        
        // left side - analog clock
        Item {
            Layout.alignment: Qt.AlignHCenter
            width: 165
            height: 190
            AnalogClock {
                id: analogClock
                dateTime: currentDate 
                clockRadius: 80
            }
        }
        
        // right side - digital clock + location
        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter
            Text {
                Layout.alignment: Qt.AlignRight
                id: timeText
                font.pointSize: 30
                font.family: clockFont.name
                color: Kirigami.Theme.highlightColor
                text: settings.use24HourTime ? Qt.formatTime(new Date(), "hh:mm") : Qt.formatTime(new Date(), "h:mm ap")
            }
            Text {
                Layout.alignment: Qt.AlignRight
                font.pointSize: 12
                text: utilModel.tzName
                color: Kirigami.Theme.textColor
            }
        }
    }
    
    Timer {
        id: timer
        interval: 100
        repeat: true
        running: true
        onTriggered: {
            timeText.text = settings.use24HourTime ? Qt.formatTime(new Date(), "hh:mm") : Qt.formatTime(new Date(), "h:mm ap")
            currentDate = new Date();
        }
    }
    
    // time zones
    ListView {
        model: timeZoneShowModel
        anchors.top: bigTimeDisplay.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        clip: true
        
        ScrollBar.vertical: ScrollBar {}
        
        delegate: timeZoneDelegate
    }
    
    PageTimezoneSelect {
        id: pagetimezone
        visible: false
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
    
    mainAction: Kirigami.Action {
        iconName: "globe"
        text: i18n("Edit")
        onTriggered: {
            pageStack.push(pagetimezone)
        }
    }
}
