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
import QtQuick.Window 2.11
import org.kde.kirigami 2.4 as Kirigami

Kirigami.ScrollablePage {
    id: stopwatchpage
    
    title: i18n("Stopwatch")
    icon.name: "chronometer"
    
    property bool running: false
    property int elapsedTime: stopwatchTimer.elapsedTime
    
    Layout.fillWidth: true

    // start/pause button
    mainAction: Kirigami.Action {
        text: running ? i18n("Pause") : i18n("Start")
        iconName: running ? "chronometer-pause" : "chronometer-start"
        onTriggered: {
            running = !running;
            stopwatchTimer.toggle();
        }
    }
    
    header: ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: Kirigami.Units.gridUnit
        spacing: Kirigami.Units.gridUnit

        // clock display
        RowLayout {
            id: timeLabels
            Layout.alignment: Qt.AlignHCenter

            Label {
                id: minutesText
                text: stopwatchTimer.minutes
                font.pointSize: Kirigami.Theme.defaultFont.pointSize*4
                font.family: clockFont.name
                font.weight: Font.Light
            }
            Label {
                text: ":"
                font.pointSize: Kirigami.Theme.defaultFont.pointSize*4
                font.family: clockFont.name
                font.weight: Font.Light
            }
            Label {
                text: stopwatchTimer.seconds
                font.pointSize: Kirigami.Theme.defaultFont.pointSize*4
                font.family: clockFont.name
                font.weight: Font.Light
            }
            Label {
                text: "."
                font.pointSize: Kirigami.Theme.defaultFont.pointSize*4
                font.family: clockFont.name
                font.weight: Font.Light
            }
            Rectangle {
                height: minutesText.height / 2
                width: Kirigami.Theme.defaultFont.pointSize*5
                color: "transparent"
                Label {
                    id: secondsText
                    text: stopwatchTimer.small
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize*2.6
                    font.family: clockFont.name
                    font.weight: Font.Light
                }
            }
        }

        // start/pause and lap button
        RowLayout {
            id: buttons
            Layout.fillWidth: true
            Layout.bottomMargin: Kirigami.Units.gridUnit
            
            Item { Layout.fillWidth: true }
            Button {
                implicitHeight: Kirigami.Units.gridUnit * 2
                implicitWidth: Kirigami.Units.gridUnit * 6
                Layout.alignment: Qt.AlignHCenter
                
                icon.name: "chronometer-reset"
                text: i18n("Reset")
                
                onClicked: {
                    running = false;
                    stopwatchTimer.reset();
                    roundModel.clear();
                    focus = false; // prevent highlight
                }
            }
            Item { Layout.fillWidth: true }
            Button {
                implicitHeight: Kirigami.Units.gridUnit * 2
                implicitWidth: Kirigami.Units.gridUnit * 6
                Layout.alignment: Qt.AlignHCenter
                
                icon.name: "chronometer-lap"
                text: i18n("Lap")
                enabled: running
                
                onClicked: {
                    roundModel.insert(0, { time: elapsedTime })
                    focus = false; // prevent highlight
                }
            }
            Item { Layout.fillWidth: true }
        }
    }
    
    // lap list display
    ListView {
        model: roundModel
        spacing: 0

        reuseItems: true
        
        ListModel {
            id: roundModel
        }
        
        // live count entry
        header: Kirigami.BasicListItem {
            visible: roundModel.count > 0
            activeBackgroundColor: "transparent"
            contentItem: RowLayout {
                Item { Layout.fillWidth: true }
                RowLayout {
                    Layout.maximumWidth: Kirigami.Units.gridUnit * 16
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 16
                    
                    Item {
                        Layout.fillHeight: true
                        Layout.leftMargin: Kirigami.Units.largeSpacing
                        implicitWidth: Kirigami.Units.gridUnit * 2
                        Label {
                            color: Kirigami.Theme.textColor
                            font.weight: Font.Bold
                            text: i18n("#%1", roundModel.count+1)
                        }
                    }
                    Label {
                        Layout.alignment: Qt.AlignLeft
                        color: Kirigami.Theme.textColor
                        text: roundModel.count == 0 ? "" : "+" + parseFloat((elapsedTime - roundModel.get(0).time)/1000).toFixed(2)
                    }
                    Item { Layout.fillWidth: true }
                    Item {
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignRight
                        implicitWidth: Kirigami.Units.gridUnit * 3
                        Label {
                            anchors.left: parent.left
                            color: Kirigami.Theme.focusColor
                            text: parseFloat(elapsedTime/1000).toFixed(2)
                        }
                    }
                }
                Item { Layout.fillWidth: true }
            }
        }
        
        // lap items
        delegate: Kirigami.BasicListItem {
            activeBackgroundColor: "transparent"

            contentItem: RowLayout {
                Item { Layout.fillWidth: true }
                
                RowLayout {
                    Layout.maximumWidth: Kirigami.Units.gridUnit * 16
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 16
                    
                    // lap number
                    Item {
                        Layout.fillHeight: true
                        Layout.leftMargin: Kirigami.Units.largeSpacing
                        Layout.minimumWidth: Kirigami.Units.gridUnit * 2
                        Label {
                            color: Kirigami.Theme.textColor
                            font.weight: Font.Bold
                            text: i18n("#%1", roundModel.count - model.index)
                        }
                    }
                    
                    // time since last lap
                    Label {
                        Layout.alignment: Qt.AlignLeft
                        color: Kirigami.Theme.textColor
                        text: {
                            if (index == roundModel.count - 1) {
                                return "+" + parseFloat(model.time/1000).toFixed(2);
                            } else if (model) {
                                return "+" + parseFloat((model.time - roundModel.get(index+1).time)/1000).toFixed(2);
                            }
                        }
                    }
                    
                    Item { Layout.fillWidth: true }
                    
                    // time since beginning
                    Item {
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignRight
                        Layout.minimumWidth: Kirigami.Units.gridUnit * 3
                        Label {
                            anchors.left: parent.left
                            color: Kirigami.Theme.focusColor
                            text: parseFloat(model.time/1000).toFixed(2)
                        }
                    }
                }
                
                Item { Layout.fillWidth: true }
            }
        }
    }
}
