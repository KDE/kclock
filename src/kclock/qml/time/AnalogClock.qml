/*
 * Copyright 2012 Viranch Mehta <viranch.mehta@gmail.com>
 * Copyright 2012 Marco Martin <mart@kde.org>
 * Copyright 2013 David Edmundson <davidedmundson@kde.org>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.4

import org.kde.plasma.core 2.0 as PlasmaCore

Item {
    id: representation

    property double hours: 0
    property double minutes: 0
    property double seconds: 0
    
    property int realHours
    property int realMinutes
    property int realSeconds
    
    // update time each second
    Timer {
        running: true
        repeat: true
        triggeredOnStart: true
        interval: 1000
        onTriggered: {
            let date = new Date();
            realHours = date.getHours();
            realMinutes = date.getMinutes();
            realSeconds = date.getSeconds();
        }
    }
    
    // open dial hands moving animation
    NumberAnimation on hours {
        id: hoursShowAnimation
        to: new Date().getHours()
        running: true
        duration: 1500
        easing.type: Easing.OutQuint
        onFinished: representation.hours = Qt.binding(() => representation.realHours)
    }
    NumberAnimation on minutes {
        id: minutesShowAnimation
        to: new Date().getMinutes()
        running: true
        duration: 1500
        easing.type: Easing.OutQuint
        onFinished: representation.minutes = Qt.binding(() => representation.realMinutes)
    }
    NumberAnimation on seconds {
        id: secondsShowAnimation
        to: new Date().getSeconds()
        running: true
        duration: 1500
        easing.type: Easing.OutQuint
        onFinished: representation.seconds = Qt.binding(() => representation.realSeconds)
    }

    PlasmaCore.Svg {
        id: clockSvg
        imagePath: "widgets/clock"
        function estimateHorizontalHandShadowOffset() {
            var id = "hint-hands-shadow-offset-to-west";
            if (hasElement(id)) {
                return -elementSize(id).width;
            }
            id = "hint-hands-shadows-offset-to-east";
            if (hasElement(id)) {
                return elementSize(id).width;
            }
            return 0;
        }
        function estimateVerticalHandShadowOffset() {
            var id = "hint-hands-shadow-offset-to-north";
            if (hasElement(id)) {
                return -elementSize(id).height;
            }
            id = "hint-hands-shadow-offset-to-south";
            if (hasElement(id)) {
                return elementSize(id).height;
            }
            return 0;
        }
        property double naturalHorizontalHandShadowOffset: estimateHorizontalHandShadowOffset()
        property double naturalVerticalHandShadowOffset: estimateVerticalHandShadowOffset()
        onRepaintNeeded: {
            naturalHorizontalHandShadowOffset = estimateHorizontalHandShadowOffset();
            naturalVerticalHandShadowOffset = estimateVerticalHandShadowOffset();
        }
    }

    Item {
        id: clock
        anchors.fill: parent
        readonly property double svgScale: face.width / face.naturalSize.width
        readonly property double horizontalShadowOffset:
            Math.round(clockSvg.naturalHorizontalHandShadowOffset * svgScale) + Math.round(clockSvg.naturalHorizontalHandShadowOffset * svgScale) % 2
        readonly property double verticalShadowOffset:
            Math.round(clockSvg.naturalVerticalHandShadowOffset * svgScale) + Math.round(clockSvg.naturalVerticalHandShadowOffset * svgScale) % 2

        PlasmaCore.SvgItem {
            id: face
            anchors.centerIn: parent
            width: Math.min(parent.width, parent.height)
            height: Math.min(parent.width, parent.height)
            svg: clockSvg
            elementId: "ClockFace"
        }

        AnalogClockHand {
            elementId: "HourHandShadow"
            rotationCenterHintId: "hint-hourhandshadow-rotation-center-offset"
            horizontalRotationOffset: clock.horizontalShadowOffset
            verticalRotationOffset: clock.verticalShadowOffset
            rotation: 180 + hours * 30 + (minutes/2)
            svgScale: clock.svgScale
            animateRotation: !hoursShowAnimation.running
        }
        AnalogClockHand {
            elementId: "HourHand"
            rotationCenterHintId: "hint-hourhand-rotation-center-offset"
            rotation: 180 + hours * 30 + (minutes/2)
            svgScale: clock.svgScale
            animateRotation: !hoursShowAnimation.running
        }

        AnalogClockHand {
            elementId: "MinuteHandShadow"
            rotationCenterHintId: "hint-minutehandshadow-rotation-center-offset"
            horizontalRotationOffset: clock.horizontalShadowOffset
            verticalRotationOffset: clock.verticalShadowOffset
            rotation: 180 + minutes * 6
            svgScale: clock.svgScale
            animateRotation: !minutesShowAnimation.running
        }
        AnalogClockHand {
            elementId: "MinuteHand"
            rotationCenterHintId: "hint-minutehand-rotation-center-offset"
            rotation: 180 + minutes * 6
            svgScale: clock.svgScale
            animateRotation: !minutesShowAnimation.running
        }

        AnalogClockHand {
            elementId: "SecondHandShadow"
            rotationCenterHintId: "hint-secondhandshadow-rotation-center-offset"
            horizontalRotationOffset: clock.horizontalShadowOffset
            verticalRotationOffset: clock.verticalShadowOffset
            rotation: 180 + seconds * 6
            svgScale: clock.svgScale
            animateRotation: !secondsShowAnimation.running
        }
        AnalogClockHand {
            elementId: "SecondHand"
            rotationCenterHintId: "hint-secondhand-rotation-center-offset"
            rotation: 180 + seconds * 6
            svgScale: clock.svgScale
            animateRotation: !secondsShowAnimation.running
        }

        PlasmaCore.SvgItem {
            id: center
            width: naturalSize.width * clock.svgScale
            height: naturalSize.height * clock.svgScale
            anchors.centerIn: clock
            svg: clockSvg
            elementId: "HandCenterScrew"
            z: 1000
        }

        PlasmaCore.SvgItem {
            anchors.fill: face
            svg: clockSvg
            elementId: "Glass"
            width: naturalSize.width * clock.svgScale
            height: naturalSize.height * clock.svgScale
        }
    }
}
