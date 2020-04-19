/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
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
import org.kde.kirigami 2.4 as Kirigami

Kirigami.SwipeListItem {
    
    property alias tzId: timeZoneId.text
    property alias tzRelative: timeZoneRelative.text
    property alias tzTime: timeZoneTime.text
    
    contentItem: Item {
        Layout.fillWidth: true
        implicitHeight: timeZoneColumn.height
        
        GridLayout {
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            
            ColumnLayout {
                id: timeZoneColumn
                Label {
                    id: timeZoneId
                    font.weight: Font.Bold
                }
                Label {
                    id: timeZoneRelative
                    font.weight: Font.Normal
                }
            }
            
            Label {
                id: timeZoneTime
                Layout.alignment: Qt.AlignRight|Qt.AlignVCenter
                font.weight: Font.Light
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.25
            }
        }
    }
}
