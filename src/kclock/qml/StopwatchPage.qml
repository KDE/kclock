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
import QtQuick.Window 2.11
import org.kde.kirigami 2.4 as Kirigami

Kirigami.ScrollablePage {
    id: stopwatchpage
    title: "Stopwatch"
    icon.name: "chronometer"
    property bool running: false
    property int elapsedTime: stopwatchTimer.elapsedTime
    
    Layout.fillWidth: true

    // start/pause button
    mainAction: Kirigami.Action {
        text: running ? "Pause" : "Start"
        iconName: running ? "chronometer-pause" : "chronometer-start"
        onTriggered: {
            running = !running;
            stopwatchTimer.toggle();
        }
    }
    
    // lap list display
    ListView {
        model: roundModel
        spacing: 0
        
        header: Item {
            height: Kirigami.Units.gridUnit * 9
            anchors.left: parent.left
            anchors.right: parent.right

            Column {
                spacing: Kirigami.Units.gridUnit
                anchors.fill: parent
                anchors.topMargin: Kirigami.Units.gridUnit
                
                // clock display
                RowLayout {
                    id: timeLabels
                    anchors.horizontalCenter: parent.horizontalCenter

                    Label {
                        id: minutesText
                        text: stopwatchTimer.minutes
                        color: Kirigami.Theme.highlightColor
                        font.pointSize: Kirigami.Theme.defaultFont.pointSize*4
                        font.family: clockFont.name
                    }
                    Label {
                        text: ":"
                        color: Kirigami.Theme.textColor
                        font.pointSize: Kirigami.Theme.defaultFont.pointSize*4
                        font.family: clockFont.name
                    }
                    Label {
                        text: stopwatchTimer.seconds
                        color: Kirigami.Theme.highlightColor
                        font.pointSize: Kirigami.Theme.defaultFont.pointSize*4
                        font.family: clockFont.name
                    }
                    Label {
                        text: "."
                        color: Kirigami.Theme.textColor
                        font.pointSize: Kirigami.Theme.defaultFont.pointSize*4
                        font.family: clockFont.name
                    }
                    Rectangle {
                        height: minutesText.height / 2
                        width: Kirigami.Theme.defaultFont.pointSize*5
                        color: "transparent"
                        Label {
                            id: secondsText
                            text: stopwatchTimer.small
                            color: Kirigami.Theme.highlightColor
                            font.pointSize: Kirigami.Theme.defaultFont.pointSize*2.6
                            font.family: clockFont.name
                        }
                    }
                }

                // start/pause and lap button
                RowLayout {
                    id: buttons
                    anchors.left: parent.left
                    anchors.right: parent.right
                    Layout.alignment: Qt.AlignHCenter

                    ToolButton {
                        Layout.alignment: Qt.AlignHCenter
                        icon.name: "chronometer-reset"
                        text: "Reset"
                        flat: false
                        onClicked: {
                            running = false;
                            stopwatchTimer.reset();
                            roundModel.clear();
                        }
                    }
                    ToolButton {
                        Layout.alignment: Qt.AlignHCenter
                        icon.name: "chronometer-lap"
                        text: "Lap"
                        flat: false
                        enabled: running
                        onClicked: {
                            roundModel.insert(0, { time: elapsedTime })
                        }
                    }
                }
            }
        }
        
        ScrollBar.vertical: ScrollBar {}

        // lap items
        delegate: Kirigami.BasicListItem {
            activeBackgroundColor: "transparent"

            contentItem: RowLayout {
                    // round number
                    Rectangle {
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignRight
                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            color: Kirigami.Theme.textColor
                            font.weight: Font.Bold
                            text: i18n("Lap") + " " + (roundModel.count - model.index)
                        }
                    }
                    Rectangle {
                        width: 1
                    }
                    // time since beginning
                    Rectangle {
                        Layout.fillHeight: true
                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            color: Kirigami.Theme.textColor
                            text: parseFloat(model.time/1000).toFixed(2)
                        }
                    }
                    // time since last lap
                    Rectangle {
                        Layout.fillHeight: true
                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            color: Kirigami.Theme.focusColor
                            text: index == roundModel.count-1 ? parseFloat(model.time/1000).toFixed(2) : "+" + parseFloat((model.time - roundModel.get(index+1).time)/1000).toFixed(2)
                        }
                    }
            }
        }
    }

    ListModel {
        id: roundModel
    }

}
