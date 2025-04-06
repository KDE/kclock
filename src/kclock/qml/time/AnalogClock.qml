/*
 * Copyright 2012 Viranch Mehta <viranch.mehta@gmail.com>
 * Copyright 2012 Marco Martin <mart@kde.org>
 * Copyright 2013 David Edmundson <davidedmundson@kde.org>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls

import org.kde.ksvg as KSvg

import org.kde.kclock as KClock

Item {
    id: representation

    property double hours: 0
    property double minutes: 0
    property double seconds: 0
    
    property int realHours
    property int realMinutes
    property int realSeconds

    onVisibleChanged: {
        if (visible) {
            updateRealTime();
            showAnimation.restart();
        }
    }

    Component.onCompleted: {
        updateRealTime();
        showAnimation.start();
    }

    function updateRealTime() {
        const date = new Date();
        realHours = date.getHours();
        realMinutes = date.getMinutes();
        realSeconds = date.getSeconds();
    }
    
    // update time each second
    Timer {
        running: representation.visible && applicationWindow().KClock.WindowExposure.exposed
        repeat: true
        triggeredOnStart: true
        interval: 1000
        onTriggered: representation.updateRealTime()
    }
    
    // open dial hands moving animation
    SequentialAnimation {
        id: showAnimation

        ScriptAction {
            script: clock.animateHands = false
        }

        ParallelAnimation {
            NumberAnimation {
                target: representation
                property: "hours"
                from: 0
                to: representation.realHours
                duration: 1500
                easing.type: Easing.OutQuint
            }
            NumberAnimation {
                target: representation
                property: "minutes"
                from: 0
                to: representation.realMinutes
                duration: 1500
                easing.type: Easing.OutQuint
            }
            NumberAnimation {
                target: representation
                property: "seconds"
                from: 0
                // By the time the animation finishes, the clock will have advanced a bit.
                to: (representation.realSeconds + 1) % 60
                duration: 1500
                easing.type: Easing.OutQuint
            }
        }

        ScriptAction {
            script: {
                representation.updateRealTime();
                clock.animateHands = true;

                representation.hours = Qt.binding(() => representation.realHours);
                representation.minutes = Qt.binding(() => representation.realMinutes);
                representation.seconds = Qt.binding(() => representation.realSeconds);
            }
        }
    }

    KSvg.Svg {
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

        Component.onCompleted: {
            KClock.UtilModel.applyPlasmaImageSet(this);
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
        property bool animateHands: false

        KSvg.SvgItem {
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
            rotation: 180 + representation.hours * 30 + (minutes/2)
            svgScale: clock.svgScale
            animateRotation: clock.animateHands
        }
        AnalogClockHand {
            elementId: "HourHand"
            rotationCenterHintId: "hint-hourhand-rotation-center-offset"
            rotation: 180 + representation.hours * 30 + (minutes/2)
            svgScale: clock.svgScale
            animateRotation: clock.animateHands
        }

        AnalogClockHand {
            elementId: "MinuteHandShadow"
            rotationCenterHintId: "hint-minutehandshadow-rotation-center-offset"
            horizontalRotationOffset: clock.horizontalShadowOffset
            verticalRotationOffset: clock.verticalShadowOffset
            rotation: 180 + representation.minutes * 6
            svgScale: clock.svgScale
            animateRotation: clock.animateHands
        }
        AnalogClockHand {
            elementId: "MinuteHand"
            rotationCenterHintId: "hint-minutehand-rotation-center-offset"
            rotation: 180 + representation.minutes * 6
            svgScale: clock.svgScale
            animateRotation: clock.animateHands
        }

        AnalogClockHand {
            elementId: "SecondHandShadow"
            rotationCenterHintId: "hint-secondhandshadow-rotation-center-offset"
            horizontalRotationOffset: clock.horizontalShadowOffset
            verticalRotationOffset: clock.verticalShadowOffset
            rotation: 180 + representation.seconds * 6
            svgScale: clock.svgScale
            animateRotation: clock.animateHands
        }
        AnalogClockHand {
            elementId: "SecondHand"
            rotationCenterHintId: "hint-secondhand-rotation-center-offset"
            rotation: 180 + representation.seconds * 6
            svgScale: clock.svgScale
            animateRotation: clock.animateHands
        }

        KSvg.SvgItem {
            id: center
            width: naturalSize.width * clock.svgScale
            height: naturalSize.height * clock.svgScale
            anchors.centerIn: clock
            svg: clockSvg
            elementId: "HandCenterScrew"
            z: 1000
        }

        KSvg.SvgItem {
            anchors.fill: face
            svg: clockSvg
            elementId: "Glass"
            width: naturalSize.width * clock.svgScale
            height: naturalSize.height * clock.svgScale
        }
    }
}
