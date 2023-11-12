// SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Templates as T

import org.kde.kirigami as Kirigami

T.SpinBox {
    id: root
    stepSize: 1

    Kirigami.Theme.colorSet: Kirigami.Theme.Button
    Kirigami.Theme.inherit: false
    
    font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 2)
    font.weight: Font.Light
    
    implicitWidth: Kirigami.Units.gridUnit * 4
    implicitHeight: up.indicator.implicitHeight + down.indicator.implicitHeight + contentItem.implicitHeight
    
    readonly property color buttonColor: Kirigami.Theme.backgroundColor
    readonly property color buttonHoverColor: Qt.darker(buttonColor, 1.05)
    readonly property color buttonPressedColor: Qt.darker(buttonColor, 1.2)
    readonly property color buttonBorderColor: Qt.alpha(Kirigami.Theme.textColor, 0.3)
    
    hoverEnabled: true
    
    validator: IntValidator {
        locale: root.locale.name
        bottom: Math.min(root.from, root.to)
        top: Math.max(root.from, root.to)
    }

    contentItem: TextInput {
        height: implicitHeight - root.up.indicator.implicitHeight - root.down.indicator.implicitHeight 
        text: root.displayText.length == 1 ? '0' + root.displayText : root.displayText
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
        z: 1
        implicitWidth: root.implicitWidth
        implicitHeight: Kirigami.Units.iconSizes.smallMedium + Kirigami.Units.largeSpacing
        color: upButton.pressed ? root.buttonPressedColor : (root.up.hovered ? root.buttonHoverColor : root.buttonColor)
        
        corners.topLeftRadius: Kirigami.Units.smallSpacing
        corners.topRightRadius: Kirigami.Units.smallSpacing
        border.color: 'transparent'
        border.width: 1

        Rectangle {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            color: root.buttonBorderColor
            height: 1
        }
        
        AbstractButton {
            id: upButton
            anchors.fill: parent
            onClicked: {
                root.increase();
                root.valueModified();
            }
        }

        Label {
            text: "+" 
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.1
            anchors.centerIn: parent
            color: enabled ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
    
    down.indicator: Kirigami.ShadowedRectangle {
        y: root.height - implicitHeight
        x: 0
        z: 1
        implicitWidth: root.implicitWidth
        implicitHeight: Kirigami.Units.iconSizes.smallMedium + Kirigami.Units.largeSpacing
        color: downButton.pressed ? root.buttonPressedColor : (root.down.hovered ? root.buttonHoverColor : root.buttonColor)
        
        corners.bottomLeftRadius: Kirigami.Units.smallSpacing
        corners.bottomRightRadius: Kirigami.Units.smallSpacing
        border.color: 'transparent'
        border.width: 1

        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            color: root.buttonBorderColor
            height: 1
        }

        AbstractButton {
            id: downButton
            anchors.fill: parent
            onClicked: {
                root.decrease();
                root.valueModified();
            }
        }
        
        Label {
            text: "-"
            font.weight: Font.Light
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.1
            anchors.centerIn: parent
            color: enabled ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
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
