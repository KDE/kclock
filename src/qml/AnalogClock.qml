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

Shape {
    property int clockRadius: 80
    property date dateTime: new Date()
    
    id: analogClock
    implicitWidth: clockRadius*2+5
    implicitHeight: clockRadius*2+5
    anchors.horizontalCenter: parent.horizontalCenter
    layer.enabled: true
    layer.samples: 40
    
    // centre dot
    ShapePath {
        id: circleCentre
        fillColor: "lightgrey"
        strokeColor: "grey"
        
        PathAngleArc {
            id: circleCentreArc
            centerX: analogClock.width / 2; centerY: analogClock.height / 2;
            radiusX: 5; radiusY: 5
            startAngle: 0
            sweepAngle: 360
        }
    }
    
    // second hand
    Rectangle {
        color: "red"
        width: 1; height: clockRadius * 0.8
        x: circleCentreArc.centerX - width / 2
        y: circleCentreArc.centerY - height;
        z: -1
        antialiasing: true
        transform: Rotation {
            origin.x: 0; origin.y: clockRadius*0.8;
            angle: (360 / 60) * dateTime.getSeconds()
            Behavior on angle {
                SpringAnimation { spring: 2; damping: 0.2; modulus: 360 }
            }
        }
    }
    
    // minute hand
    Rectangle {
        color: "black"
        width: 3; height: clockRadius * 0.7
        x: circleCentreArc.centerX + Math.sin(minuteRotation.angle * Math.PI / 180) * width
        y: circleCentreArc.centerY - height - Math.sin(minuteRotation.angle * Math.PI / 180) * width;
        z: -1
        antialiasing: true
        transform: Rotation {
            id: minuteRotation
            origin.x: 0; origin.y: clockRadius * 0.7;
            angle: (360 / 60) * dateTime.getMinutes() + (360 / 3600) * dateTime.getSeconds()
            Behavior on angle {
                SpringAnimation { spring: 2; damping: 0.2; modulus: 360 }
            }
        }
    }
    
    // hour hand
    Rectangle {
        color: "black"
        width: 3; height: clockRadius * 0.4
        x: circleCentreArc.centerX + Math.sin(hourRotation.angle * Math.PI / 180) * width
        y: circleCentreArc.centerY - height - Math.sin(minuteRotation.angle * Math.PI / 180) * width;
        z: -1
        antialiasing: true
        transform: Rotation {
            id: hourRotation
            origin.x: 0; origin.y: clockRadius * 0.4;
            angle: (360 / 12) * dateTime.getHours() + (360 / (12*60)) * dateTime.getMinutes()
            Behavior on angle {
                SpringAnimation { spring: 2; damping: 0.2; modulus: 360 }
            }
        }
    }
    
    // dashes for each hour
    Repeater {
        model: 12
        Rectangle {
            color: "grey"
            width: 3; height: 6
            antialiasing: true
            x: circleCentreArc.centerX-width/2; y: circleCentreArc.centerY - clockRadius + height / 2
            transform: Rotation {
                origin.x: width / 2; origin.y: clockRadius
                angle: (360 / 12) * modelData
            }
        }
    }
}
