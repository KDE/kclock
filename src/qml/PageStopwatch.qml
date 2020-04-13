/*
 * Copyright 2019  Nick Reitemeyer <nick.reitemeyer@web.de>
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
            color: Kirigami.Theme.focusColor
            font.pointSize: 40
            font.kerning: false
        }
        Text {
            text: ":"
            color: Kirigami.Theme.textColor
            font.pointSize: 40
        }
        Label {
            text: getElapsedSeconds()
            color: Kirigami.Theme.focusColor
            font.pointSize: 40
            font.kerning: false
        }
        Text {
            text: "."
            color: Kirigami.Theme.textColor
            font.pointSize: 40
        }
        Rectangle {
            height: minutesText.height / 2
            width: 50
            Text {
                id: secondsText
                text: getElapsedSmall()
                color: Kirigami.Theme.focusColor
                font.pointSize: 26
                font.kerning: false
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
            text: running ? "Pause" : "Start"
            icon.name: running ? "chronometer-pause" : "chronometer-start"
            onClicked: {
                running = !running
            }
            Layout.alignment: Qt.AlignHCenter
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

        delegate: Kirigami.BasicListItem {
            activeBackgroundColor: "transparent" // Kirigami.Theme.backgroundColor

            contentItem: RowLayout { // TODO don't put layout as contentItem
                    Rectangle {
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignRight
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "<b>#" + (roundModel.count - model.index) + "</b>"
                        }
                    }
                    Rectangle {
                        width: 1
                    }
                    Rectangle {
                        Layout.fillHeight: true

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: parseFloat(model.time/1000).toFixed(2)
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
        iconName: "chronometer-reset"
        text: "Reset"
        tooltip: "Reset"
        onTriggered: {
            running = false;
            elapsedTime = 0
            roundModel.clear()
        }
    }
}
