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
import QtQuick.Shapes 1.12
import org.kde.kirigami 2.11 as Kirigami

Rectangle {
    anchors.fill: parent
    
    property int timerDuration
    property int timerElapsed
    
    color: "transparent"
    
    function getTimeLeft() {
        return timerDuration*1000 - timerElapsed;
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
    
    // timer circle
    Shape {
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
                sweepAngle: 360 * (timerElapsed / 1000) / timerDuration
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
                startAngle: (-90 + 360 * (timerElapsed % 1000) / 1000) % 360
                sweepAngle: 16
            }
        }
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
