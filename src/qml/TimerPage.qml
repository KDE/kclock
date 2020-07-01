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
import kclock 1.0

Kirigami.Page {
    
    property Timer timer
    
    id: timerpage
    title: timer.label
    
    property bool justCreated: true
    
    property int elapsed: timer == null ? 0 : timer.elapsed
    property int duration: timer == null ? 0 : timer.length / 1000
    property bool running: timer == null ? 0 : timer.running

    // topbar action
    mainAction: Kirigami.Action {
        text: running ? "Pause" : "Start"
        iconName: running ? "chronometer-pause" : "chronometer-start"
        onTriggered: timer.toggleRunning()
        visible: !justCreated
    }

    function getTimeLeft() {
        return duration*1000 - elapsed;
    }
    function getHours() {
        return ("0" + parseInt(getTimeLeft() / 1000 / 60 / 60).toFixed(0)).slice(-2);
    }
    function getMinutes() {
        return ("0" + parseInt(getTimeLeft() / 1000 / 60 - 60 * getHours())).slice(-2);
    }
    function getSeconds() {
        return ("0" + parseInt(getTimeLeft() / 1000 - 60 * getMinutes())).slice(-2);
    }

    // create new timer form
    ColumnLayout {
        spacing: Kirigami.Units.largeSpacing
        visible: justCreated
        anchors.left: parent.left
        anchors.right: parent.right
        
        Kirigami.FormLayout {
            id: form
            Layout.fillWidth: true
            
            Item {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: "Duration"
            }
            Kirigami.Separator {}
            
            RowLayout {
                SpinBox {
                    Layout.alignment: Qt.AlignVCenter
                    id: spinBoxHours
                    onValueChanged: form.setDuration()
                    value: duration / 60 / 60
                }
                Label {
                    Layout.alignment: Qt.AlignVCenter
                    text: i18n("hours")
                }
            }
            RowLayout {
                SpinBox {
                    Layout.alignment: Qt.AlignVCenter
                    id: spinBoxMinutes
                    onValueChanged: form.setDuration()
                    value: duration % (60*60) / 60
                }
                Label {
                    Layout.alignment: Qt.AlignVCenter
                    text: i18n("minutes")
                }
            }
            RowLayout {
                SpinBox {
                    Layout.alignment: Qt.AlignVCenter
                    id: spinBoxSeconds
                    to: 60
                    onValueChanged: form.setDuration()
                    value: duration % 60
                }
                Label {
                    Layout.alignment: Qt.AlignVCenter
                    text: i18n("seconds")
                }
            }
            
            function setDuration() {
                timer.length = 1000 * (spinBoxHours.value * 60 * 60 + spinBoxMinutes.value * 60 + spinBoxSeconds.value)
            }
            
            Item {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: "Label (optional)"
            }
            
            Kirigami.Separator {}
            TextField {
                text: timer.label
                onTextChanged: timer.label = text
            }
            
            Kirigami.Separator {}
            ToolButton {
                flat: false
                icon.name: "chronometer-start"
                text: "Start"
                onClicked: {
                    timer.toggleRunning();
                    justCreated = false;
                }
            }
        }   
    }

    
    
    // ~ timer display ~
    
    // timer circle
    Shape {
        visible: !justCreated
        
        anchors.centerIn: parent
        
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
                radiusX: Math.max(timerpage.width * 0.25, 1); radiusY: radiusX
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
                sweepAngle: 360 * (elapsed / 1000) / duration
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
                startAngle: (-90 + 360 * (elapsed % 1000) / 1000) % 360
                sweepAngle: 16
            }
        }
    }
    
    // clock display
    RowLayout {
        visible: !justCreated
        id: timeLabels
        anchors.centerIn: timerCircle

        Label {
            id: hoursText
            text: getHours()
            color: Kirigami.Theme.highlightColor
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*3
            font.family: clockFont.name
            visible: text != "00"
        }
        Label {
            text: ":"
            color: Kirigami.Theme.textColor
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*3
            font.family: clockFont.name
            visible: getHours() != "00"
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

}
