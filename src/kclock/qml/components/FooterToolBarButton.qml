/*
 * SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.12 as Kirigami

Controls.AbstractButton {
    id: button
    
    property real iconSize: 24
    
    readonly property color activeColor: Qt.rgba(Kirigami.Theme.disabledTextColor.r, Kirigami.Theme.disabledTextColor.g, Kirigami.Theme.disabledTextColor.b, 0.3)
    readonly property color hoverColor: Qt.rgba(Kirigami.Theme.disabledTextColor.r, Kirigami.Theme.disabledTextColor.g, Kirigami.Theme.disabledTextColor.b, 0.2)
    
    background: Rectangle {
        Kirigami.Theme.colorSet: Kirigami.Theme.Button
        Kirigami.Theme.inherit: false
        color: (button.checked || button.pressed) ? activeColor : (hoverHandler.hovered ? hoverColor : "transparent")
    }
    
    contentItem: Item {
        implicitHeight: Kirigami.Units.gridUnit * (button.display !== Controls.AbstractButton.TextOnly ? 3 : 2.75)
        implicitWidth: Kirigami.Units.gridUnit * 3.5
        
        Behavior on implicitHeight { 
            NumberAnimation {
                duration: Kirigami.Units.longDuration 
                easing.type: Easing.InOutQuad
            } 
        }
        
        Kirigami.Icon {
            isMask: true // no coloured icons (should all be symbolic actions)
            anchors.centerIn: parent
            transform: Translate { 
                y: label.opacity !== 0 ? -Kirigami.Units.smallSpacing * 2 : 0 
                Behavior on y { 
                    NumberAnimation {
                        duration: Kirigami.Units.longDuration 
                        easing.type: Easing.InOutQuad
                    } 
                }
            }
            implicitHeight: button.iconSize
            implicitWidth: height
            visible: button.icon.name
            source: button.icon.name
        }
        
        Controls.Label {
            id: label
            anchors.bottom: parent.bottom
            anchors.bottomMargin: Kirigami.Units.smallSpacing
            anchors.horizontalCenter: parent.horizontalCenter
            text: button.text
            font: Kirigami.Theme.smallFont
            horizontalAlignment: Text.AlignHCenter
            opacity: button.display !== Controls.AbstractButton.TextOnly ? 1 : 0
            
            Behavior on opacity { 
                NumberAnimation {
                    duration: Kirigami.Units.longDuration 
                    easing.type: Easing.InOutQuad
                } 
            }
        }
        
        HoverHandler {
            id: hoverHandler
        }
    }
}
