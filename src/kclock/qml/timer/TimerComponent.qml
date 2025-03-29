/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes 1.12

import org.kde.kirigami as Kirigami

Item {
    id: root
    property int timerDuration
    property int timerElapsed
    property bool timerRunning
    
    signal requestAddMinute()
    
    function getCircleRadius(): double {
        const totalHeight = heading.height + timeLabels.height + addMinuteButton.height;
        const maxWidth = Math.max(heading.width, timeLabels.width, addMinuteButton.width);

        const contentDiag = Math.sqrt(totalHeight ** 2 + maxWidth ** 2);
        const maxRadius = Math.min(root.width, root.height) * (Kirigami.Settings.isMobile ? 0.3 : 0.25);
        
        return Math.max(maxRadius, contentDiag / 2);
    }
    
    function getTimeLeft() {
        return timerDuration - timerElapsed;
    }
    function getHours() {
        return ("0" + parseInt(getTimeLeft() / 60 / 60).toFixed(0)).slice(-2);
    }
    function getMinutes() {
        return ("0" + parseInt(getTimeLeft() / 60 - 60 * getHours())).slice(-2);
    }
    function getSeconds() {
        return ("0" + parseInt(getTimeLeft() - 60 * getMinutes())).slice(-2);
    }
    
    // elapsed sweep angle animation (progress circle)
    property int elapsedSweepAngle
    NumberAnimation on elapsedSweepAngle {
        id: elapsedSweepAnimation
        easing.type: Easing.InOutQuad
        duration: 500
    }
    onTimerElapsedChanged: {
        elapsedSweepAnimation.to = 360 * timerElapsed / timerDuration
        elapsedSweepAnimation.start();
    }
    
    // set initial values
    Component.onCompleted: {
        elapsedSweepAngle = 360 * timerElapsed / timerDuration;
    }
    
    // timer circle
    Shape {
        anchors.centerIn: parent
        
        id: timerCircle
        implicitWidth: timerCircleArc.radiusX * 2 + timerCirclePath.strokeWidth
        implicitHeight: timerCircleArc.radiusY * 2 + timerCirclePath.strokeWidth
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
                radiusX: getCircleRadius()
                radiusY: radiusX
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
                sweepAngle: elapsedSweepAngle
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
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*3
            font.weight: Font.Light
            color: timerRunning ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
            visible: text != "00"
        }
        Label {
            text: ":"
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*3
            font.weight: Font.Light
            color: timerRunning ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
            visible: getHours() != "00"
        }
        Label {
            id: minutesText
            text: getMinutes()
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*3
            font.weight: Font.Light
            color: timerRunning ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
        }
        Label {
            text: ":"
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*3
            font.weight: Font.Light
            color: timerRunning ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
        }
        Label {
            text: getSeconds()
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*3
            font.weight: Font.Light
            color: timerRunning ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
        }
    }
    
    Kirigami.Heading {
        id: heading
        level: 4
        text: timer.label
        color: timerRunning ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
        anchors.bottom: timeLabels.top
        anchors.bottomMargin: Kirigami.Units.smallSpacing
        anchors.horizontalCenter: parent.horizontalCenter
    }

    ToolButton {
        id: addMinuteButton
        text: i18n("1 minute")
        icon.name: "list-add"
        anchors.topMargin: Kirigami.Units.smallSpacing
        anchors.top: timeLabels.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        implicitHeight: Kirigami.Units.gridUnit * 2
        onClicked: {
            root.requestAddMinute();
        }
    }
}
