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

Kirigami.Page {
    id: stopwatchpage
    title: "Stopwatch"
    property bool running: false
    property int elapsedTime: 0
    property double displayTime: elapsedTime

    function getElapsedHours() {
        return ("0" + parseInt(elapsedTime / 1000 / 60 / 24).toFixed(0)).slice(-2);
    }
    function getElapsedMinutes() {
        return ("0" + parseInt(elapsedTime / 1000 / 60 - 24*getElapsedHours())).slice(-2);
    }
    function getElapsedSeconds() {
        return ("0" + parseInt(elapsedTime / 1000 - 60*getElapsedMinutes())).slice(-2);
    }
    function getElapsedSmall() {
        return ("0" + parseInt(elapsedTime / 10 - (60*getElapsedSeconds())).toFixed(0)).slice(-2);
    }

    // clock display
    RowLayout {
        id: timeLabels
        anchors.horizontalCenter: parent.horizontalCenter

        Label {
            id: minutesText
            text: getElapsedMinutes()
            color: Kirigami.Theme.highlightColor
            font.pointSize: 40
            font.kerning: false
            font.family: clockFont.name
        }
        Text {
            text: ":"
            color: Kirigami.Theme.textColor
            font.pointSize: 40
            font.family: clockFont.name
        }
        Label {
            text: getElapsedSeconds()
            color: Kirigami.Theme.highlightColor
            font.pointSize: 40
            font.kerning: false
            font.family: clockFont.name
        }
        Text {
            text: "."
            color: Kirigami.Theme.textColor
            font.pointSize: 40
            font.family: clockFont.name
        }
        Rectangle {
            height: minutesText.height / 2
            width: 50
            color: "transparent"
            Text {
                id: secondsText
                text: getElapsedSmall()
                color: Kirigami.Theme.highlightColor
                font.pointSize: 26
                font.kerning: false
                font.family: clockFont.name
            }
        }
    }

    // start/pause and lap button
    RowLayout {
        id: buttons
        anchors.topMargin: 20;
        anchors.top: timeLabels.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        Layout.alignment: Qt.AlignHCenter

        ToolButton {
            Layout.alignment: Qt.AlignHCenter
            icon.name: "chronometer-reset"
            text: "Reset"
            onClicked: {
                running = false;
                elapsedTime = 0
                roundModel.clear()
            }
        }
        ToolButton {
            text: "Lap"
            icon.name: "chronometer-lap"
            onClicked: {
                roundModel.insert(0, { time: elapsedTime })
            }
            Layout.alignment: Qt.AlignHCenter
        }
    }

    // lap list display
    ListView {
        anchors.top: buttons.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.topMargin: 15;
        model: roundModel
        spacing: 0
        ScrollBar.vertical: ScrollBar {}

        delegate: Kirigami.BasicListItem {
            activeBackgroundColor: "transparent"

            contentItem: RowLayout {
                    // round number
                    Rectangle {
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignRight
                        Text {
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
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            color: Kirigami.Theme.textColor
                            text: parseFloat(model.time/1000).toFixed(2)
                        }
                    }
                    // time since last lap
                    Rectangle {
                        Layout.fillHeight: true
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            color: Kirigami.Theme.focusColor
                            text: index == roundModel.count-1 ? parseFloat(model.time/1000).toFixed(2) : "+" + parseFloat((model.time - roundModel.get(index+1).time)/1000).toFixed(2)
                        }
                    }
            }
        }
    }

    // clock increment
    Timer {
        id: stopwatchTimer
        interval: 16
        running: stopwatchpage.running
        repeat: true
        onTriggered: {
            elapsedTime += interval
        }
    }

    ListModel {
        id: roundModel
    }

    // topbar action
    mainAction: Kirigami.Action {
        text: running ? "Pause" : "Start"
        iconName: running ? "chronometer-pause" : "chronometer-start"
        onTriggered: {
            running = !running
        }
        Layout.alignment: Qt.AlignHCenter
    }
}
