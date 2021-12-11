/*
 * Copyright 2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.3

import org.kde.kirigami 2.15 as Kirigami

SpinBox {
    id: root

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false
    
    font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 2)
    font.weight: Font.Light
    
    implicitWidth: Kirigami.Units.gridUnit * 4
    implicitHeight: up.indicator.implicitHeight + down.indicator.implicitHeight + contentItem.implicitHeight
    
    readonly property color buttonColor: Kirigami.Theme.backgroundColor
    readonly property color buttonHoverColor: Qt.darker(buttonColor, 1.05)
    readonly property color buttonPressedColor: Qt.darker(buttonColor, 1.1)
    readonly property color buttonBorderColor: Qt.rgba(Kirigami.Theme.textColor.r, Kirigami.Theme.textColor.g, Kirigami.Theme.textColor.b, 0.1)
    
    hoverEnabled: true
    
    contentItem: TextInput {
        height: Kirigami.Units.gridUnit * 4
        width: Kirigami.Units.gridUnit * 4
        
        z: 2
        text: root.textFromValue(root.value, root.locale)
        
        font: root.font
        
        color: Kirigami.Theme.textColor
        selectionColor: Kirigami.Theme.highlightColor
        selectedTextColor: Kirigami.Theme.highlightedTextColor
        selectByMouse: true
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter

        readOnly: !root.editable
        validator: root.validator
        inputMethodHints: Qt.ImhFormattedNumbersOnly
        
        renderType: Screen.devicePixelRatio % 1 !== 0 ? Text.QtRendering : Text.NativeRendering
        
        MouseArea {
            anchors.fill: parent
            onPressed: mouse.accepted = false;

            property int wheelDelta: 0

            onExited: wheelDelta = 0
            onWheel: {
                wheelDelta += wheel.angleDelta.y;
                // magic number 120 for common "one click"
                // See: http://qt-project.org/doc/qt-5/qml-qtquick-wheelevent.html#angleDelta-prop
                while (wheelDelta >= 120) {
                    wheelDelta -= 120;
                    root.increase();
                    root.valueModified();
                }
                while (wheelDelta <= -120) {
                    wheelDelta += 120;
                    root.decrease();
                    root.valueModified();
                }
            }

            // Normally the TextInput does this automatically, but the MouseArea on
            // top of it blocks that behavior, so we need to explicitly do it here
            cursorShape: Qt.IBeamCursor
        }
    }
    
    up.indicator: Kirigami.ShadowedRectangle {
        y: 0
        x: 0
        implicitWidth: root.implicitWidth
        implicitHeight: Kirigami.Units.iconSizes.small + Kirigami.Units.largeSpacing
        color: root.up.pressed ? root.buttonPressedColor : (root.up.hovered ? root.buttonHoverColor : root.buttonColor)
        
        corners.topLeftRadius: Kirigami.Units.smallSpacing
        corners.topRightRadius: Kirigami.Units.smallSpacing
        border.color: root.buttonBorderColor
        border.width: 1
        
        Text {
            text: "+"
            font.pixelSize: root.font.pixelSize * 2
            anchors.fill: parent
            fontSizeMode: Text.Fit
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
    
    down.indicator: Kirigami.ShadowedRectangle {
        y: root.height - implicitHeight
        x: 0
        implicitWidth: root.implicitWidth
        implicitHeight: Kirigami.Units.iconSizes.small + Kirigami.Units.largeSpacing
        color: root.down.pressed ? root.buttonPressedColor : (root.down.hovered ? root.buttonHoverColor : root.buttonColor)
        
        corners.bottomLeftRadius: Kirigami.Units.smallSpacing
        corners.bottomRightRadius: Kirigami.Units.smallSpacing
        border.color: root.buttonBorderColor
        border.width: 1
        
        Text {
            text: "-"
            font.pixelSize: root.font.pixelSize * 2
            anchors.fill: parent
            fontSizeMode: Text.Fit
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
    
    background: Rectangle {
        border.color: root.buttonBorderColor
        border.width: 1
        color: Kirigami.Theme.backgroundColor
        radius: Kirigami.Units.smallSpacing
    }
}
