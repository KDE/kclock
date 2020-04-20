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
import org.kde.kirigami 2.11 as Kirigami
import kirigamiclock 1.0

Kirigami.Page {
    
    title: "Timer"
    id: timerpage
    property bool running: false
    property bool finished: false
    property int timerDuration: 60
    property int elapsedTime: 0
    
    Timer {
        interval: 16
        running: timerpage.running
        repeat: true
        onTriggered: {
            elapsedTime += interval
            
            // if timer finished
            if (timerDuration * 1000 <= elapsedTime) {
                finishedTimer();
            }
        }
    }
    
    function finishedTimer() {
        elapsedTime = timerDuration * 1000;
        running = false;
        finished = true;
        
        timerModel.timerFinished();
    }
    
    function resetTimer() {
        running = false;
        finished = false;
        elapsedTime = 0;
    }

    // topbar action
    mainAction: Kirigami.Action {
        text: running ? "Pause" : "Start"
        iconName: running ? "chronometer-pause" : "chronometer-start"
        onTriggered: {
            if (finished) resetTimer();
            running = !running;
        }
    }
    
    // outside circle
    Shape {
        id: timerCircle
        implicitWidth: parent.width
        implicitHeight: timerCircleArc.radiusX*2+5
        anchors.horizontalCenter: parent.horizontalCenter
        layer.enabled: true
        layer.samples: 40
        
        Kirigami.Theme.colorSet: Kirigami.Theme.Button
        
        // base circle
        ShapePath {
            id: timerCirclePath
            strokeColor: "lightgrey"
            fillColor: "transparent"
            strokeWidth: 4
            capStyle: ShapePath.FlatCap
            PathAngleArc {
                id: timerCircleArc
                centerX: timerCircle.width / 2; centerY: timerCircle.height / 2;
                radiusX: Math.max(pagetimer.width * 0.25, timeLabels.width / 2 + 5); radiusY: radiusX
                startAngle: -180
                sweepAngle: 360
            }
        }
        
        // progress circle
        ShapePath {
            strokeColor: Kirigami.Theme.highlightColor
            fillColor: "transparent"
            strokeWidth: 4
            capStyle: ShapePath.FlatCap
            PathAngleArc {
                centerX: timerCircleArc.centerX; centerY: timerCircleArc.centerY
                radiusX: timerCircleArc.radiusX; radiusY: timerCircleArc.radiusY
                startAngle: -90
                sweepAngle: 360 * (elapsedTime / 1000) / timerDuration
            }
        }
        
        // lapping circle
        ShapePath {
            strokeColor: running ? "white" : "transparent"
            fillColor: "transparent"
            strokeWidth: 4
            capStyle: ShapePath.FlatCap
            PathAngleArc {
                centerX: timerCircleArc.centerX; centerY: timerCircleArc.centerY
                radiusX: timerCircleArc.radiusX; radiusY: timerCircleArc.radiusY
                startAngle: (-90 + 360 * (elapsedTime % 1000) / 1000) % 360
                sweepAngle: 16
            }
        }
    }

    function getTimeLeft() {
        return timerDuration*1000 - elapsedTime;
    }
    function getHours() {
        return ("0" + parseInt(getTimeLeft() / 1000 / 60 / 24).toFixed(0)).slice(-2);
    }
    function getMinutes() {
        return ("0" + parseInt(getTimeLeft() / 1000 / 60 - 24*getHours())).slice(-2);
    }
    function getSeconds() {
        return ("0" + parseInt(getTimeLeft() / 1000 - 60*getMinutes())).slice(-2);
    }

    // clock display
    RowLayout {
        id: timeLabels
        anchors.centerIn: timerCircle

        Label {
            id: hoursText
            text: getHours()
            color: Kirigami.Theme.highlightColor
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*3
            font.family: clockFont.name
        }
        Label {
            text: ":"
            color: Kirigami.Theme.textColor
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*3
            font.family: clockFont.name
        }
        Label {
            id: minutesText
            text: getMinutes()
            color: Kirigami.Theme.highlightColor
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*3
            font.family: clockFont.name
        }
        Label {
            text: ":"
            color: Kirigami.Theme.textColor
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*3
            font.family: clockFont.name
        }
        Label {
            text: getSeconds()
            color: Kirigami.Theme.highlightColor
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*3
            font.family: clockFont.name
        }
    }

    // start/pause and lap button
    RowLayout {
        id: buttons
        anchors.topMargin: 20;
        anchors.top: timerCircle.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        Layout.alignment: Qt.AlignHCenter

        ToolButton {
            icon.name: "chronometer"
            text: "Edit"
            onClicked: {
                timerEditSheet.open()
            }
            Layout.alignment: Qt.AlignHCenter
        }
        ToolButton {
            text: "Reset"
            icon.name: "chronometer-reset"
            onClicked: resetTimer();
            Layout.alignment: Qt.AlignHCenter
        }
    }

    Kirigami.OverlaySheet {
        id: timerEditSheet
        header: Kirigami.Heading {
            text: i18n("Change Timer Duration")
        }

        ColumnLayout {
            spacing: Kirigami.Units.largeSpacing * 5
            Layout.preferredWidth:  Kirigami.Units.gridUnit * 25
            Kirigami.FormLayout {
                Layout.fillWidth: true

                SpinBox {
                    Kirigami.FormData.label: i18n("Minutes")
                    id: spinBoxMinutes
                    onValueChanged: timerEditSheet.setDuration()
                    value: timerDuration / 60
                }

                SpinBox {
                    Kirigami.FormData.label: i18n("Seconds")
                    id: spinBoxSeconds
                    to: 60
                    onValueChanged: timerEditSheet.setDuration()
                    value: timerDuration % 60
                }
            }
        }

         function setDuration() {
             timerDuration = spinBoxMinutes.value * 60 + spinBoxSeconds.value
         }
    }
}
