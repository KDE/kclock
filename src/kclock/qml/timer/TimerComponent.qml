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

import org.kde.kclock as KClock

Item {
    id: root

    required property KClock.Timer timer

    readonly property int timerDuration: timer.length
    readonly property int timerElapsed: timer.elapsed
    readonly property bool timerRunning: timer.running

    property alias actions: actionToolBar.actions
    property bool actionsVisible: true

    property bool labelEditable

    // Whether to make the circle fill the item.
    property bool maximizedCircle: false

    function getCircleRadius(): double {
        const totalHeight = heading.implicitHeight + timeLabels.implicitHeight + actionToolBar.implicitHeight
        const maxWidth = Math.max(heading.implicitWidth, timeLabels.implicitWidth, actionToolBar.implicitWidth);

        const contentDiag = Math.sqrt(totalHeight ** 2 + maxWidth ** 2);
        const maxRadius = Math.min(root.width, root.height) * (Kirigami.Settings.isMobile ? 0.3 : 0.25);

        return Math.max(maxRadius, contentDiag / 2);
    }

    function getTimeLeft() {
        return root.timerDuration - root.timerElapsed;
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
                radiusX: root.maximizedCircle ? Math.min(root.width, root.height) / 2 : root.getCircleRadius()
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
                sweepAngle: root.elapsedSweepAngle
            }
        }
    }

    // clock display
    RowLayout {
        id: timeLabels
        anchors.centerIn: timerCircle

        Label {
            id: hoursText
            text: root.getHours()
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*3
            font.weight: Font.Light
            color: root.timerRunning ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
            visible: text !== "00"
        }
        Label {
            text: ":"
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*3
            font.weight: Font.Light
            color: root.timerRunning ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
            visible: root.getHours() !== "00"
        }
        Label {
            id: minutesText
            text: root.getMinutes()
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*3
            font.weight: Font.Light
            color: root.timerRunning ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
        }
        Label {
            text: ":"
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*3
            font.weight: Font.Light
            color: root.timerRunning ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
        }
        Label {
            text: root.getSeconds()
            font.pointSize: Kirigami.Theme.defaultFont.pointSize*3
            font.weight: Font.Light
            color: root.timerRunning ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
        }
    }
    Rectangle {
        id: timeCompletedBox
        radius: Kirigami.Units.cornerRadius
        Kirigami.Heading {
            id: timeCompletedHeading
            level: 3
            color: root.timerRunning ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
            text: root.timer.timeCompletedPretty
            anchors.centerIn: parent
            textFormat: Text.PlainText
        }
        implicitWidth: timeCompletedHeading.implicitWidth + Kirigami.Units.smallSpacing * 2
        implicitHeight: timeCompletedHeading.implicitHeight + Kirigami.Units.smallSpacing * 2
        color: Kirigami.Theme.neutralBackgroundColor
        anchors.top: timeLabels.bottom
        anchors.bottomMargin: Kirigami.Units.smallSpacing
        anchors.horizontalCenter: parent.horizontalCenter
        visible: root.timerRunning
    }

    Kirigami.Heading {
        id: heading
        level: 4
        color: root.timerRunning ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
        text: root.timer.label || i18n("New timer")
        textFormat: Text.PlainText
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
        width: Math.round(timerCircleArc.radiusX * 2 * 0.85)
        anchors.bottom: timeLabels.top
        anchors.bottomMargin: Kirigami.Units.smallSpacing
        anchors.horizontalCenter: parent.horizontalCenter
        visible: !labelField.activeFocus

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true // for ToolTip
            Accessible.name: i18n("Click to edit timer name")
            visible: !root.timerRunning && root.labelEditable
            cursorShape: Qt.IBeamCursor
            onClicked: {
                labelField.text = root.timer.label;
                labelField.forceActiveFocus();
                labelField.selectAll();
            }

            ToolTip {
                text: parent.Accessible.name
            }
        }
    }

    TextField {
        id: labelField
        anchors.fill: heading
        horizontalAlignment: Text.AlignHCenter
        background: null
        topPadding: 0
        leftPadding: 0
        rightPadding: 0
        bottomPadding: 0
        font: heading.font
        visible: activeFocus

        // NOTE This is not onAccepted to prevent the Keys.onReturnPressed
        // on TimerPage from getting the event.
        Keys.onReturnPressed: {
            root.timer.label = text;
            root.forceActiveFocus();
        }
        Keys.onEnterPressed: (event) => Keys.returnPressed(event)
        Keys.onEscapePressed: root.forceActiveFocus()
    }

    Kirigami.ActionToolBar {
        id: actionToolBar
        anchors.topMargin: Kirigami.Units.largeSpacing
        anchors.top: timerCircle.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        opacity: root.actionsVisible ? 1 : 0
        visible: opacity > 0

        Behavior on opacity {
            NumberAnimation {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }
    }
}
