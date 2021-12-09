/*
 * Copyright 2012 Viranch Mehta <viranch.mehta@gmail.com>
 * Copyright 2012 Marco Martin <mart@kde.org>
 * Copyright 2013 David Edmundson <davidedmundson@kde.org>
 * Copyright 2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0

import org.kde.plasma.core 2.0 as PlasmaCore

PlasmaCore.SvgItem {
    id: handRoot

    property bool animateRotation: true
    
    property alias rotation: rotation.angle
    property double svgScale
    property double horizontalRotationOffset: 0
    property double verticalRotationOffset: 0
    property string rotationCenterHintId
    readonly property double horizontalRotationCenter: {
        if (svg.hasElement(rotationCenterHintId)) {
            var hintedCenterRect = svg.elementRect(rotationCenterHintId),
                handRect = svg.elementRect(elementId),
                hintedX = hintedCenterRect.x - handRect.x + hintedCenterRect.width/2;
            return Math.round(hintedX * svgScale) + Math.round(hintedX * svgScale) % 2;
        }
        return width/2;
    }
    readonly property double verticalRotationCenter: {
        if (svg.hasElement(rotationCenterHintId)) {
            var hintedCenterRect = svg.elementRect(rotationCenterHintId),
                handRect = svg.elementRect(elementId),
                hintedY = hintedCenterRect.y - handRect.y + hintedCenterRect.height/2;
            return Math.round(hintedY * svgScale) + width % 2;
        }
        return width/2;
    }

    width: Math.round(naturalSize.width * svgScale) + Math.round(naturalSize.width * svgScale) % 2
    height: Math.round(naturalSize.height * svgScale) + width % 2
    anchors {
        top: clock.verticalCenter
        topMargin: -verticalRotationCenter + verticalRotationOffset
        left: clock.horizontalCenter
        leftMargin: -horizontalRotationCenter + horizontalRotationOffset
    }

    svg: clockSvg
    transform: Rotation {
        id: rotation
        angle: 0
        origin {
            x: handRoot.horizontalRotationCenter
            y: handRoot.verticalRotationCenter
        }
        Behavior on angle {
            RotationAnimation {
                id: anim
                duration: animateRotation ? 400 : 0
                direction: RotationAnimation.Clockwise
                easing.type: animateRotation ? Easing.OutElastic : Easing.Linear
                easing.overshoot: 0.5
            }
        }
    }
}
