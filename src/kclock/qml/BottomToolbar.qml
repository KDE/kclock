/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
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

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.11 as Kirigami

ToolBar {
    function getPage(name) {
        switch (name) {
            case "Time": return timePage;
            case "Timer": return timerListPage;
            case "Stopwatch": return stopwatchPage;
            case "Alarm": return alarmPage;
            case "Settings": return settingsPage;
        }
    }
    
    RowLayout {
        anchors.fill: parent
        Repeater {
            
            model: ListModel {
                ListElement {
                    name: "Time"
                    icon: "clock"
                }
                ListElement {
                    name: "Timer"
                    icon: "player-time"
                }
                ListElement {
                    name: "Stopwatch"
                    icon: "chronometer"
                }
                ListElement {
                    name: "Alarm"
                    icon: "notifications"
                }
            }
            
            ColumnLayout {
                Layout.preferredWidth: parent.width / 4
                Layout.preferredHeight: Kirigami.Units.gridUnit * 2.5
                Layout.alignment: Qt.AlignCenter
                spacing: 0
                
                Kirigami.Icon {
                    color: getPage(model.name).visible ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                    source: model.icon
                    Layout.alignment: Qt.AlignCenter
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 1.5
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 1.5
                }
                Label {
                    color: getPage(model.name).visible ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                    text: i18n(model.name)
                    Layout.alignment: Qt.AlignCenter
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 0.8
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: appwindow.switchToPage(getPage(model.name), 0)
                }
            }
        }
    }
} 
