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
    
    background: Rectangle {
        radius: Kirigami.Units.smallSpacing
        Kirigami.Theme.colorSet: Kirigami.Theme.Button
        Kirigami.Theme.inherit: false
        color: button.pressed ? Qt.rgba(Kirigami.Theme.disabledTextColor.r, Kirigami.Theme.disabledTextColor.g, Kirigami.Theme.disabledTextColor.b, 0.3) : 
                                (button.checked || hoverHandler.hovered ? Qt.rgba(Kirigami.Theme.disabledTextColor.r, Kirigami.Theme.disabledTextColor.g, Kirigami.Theme.disabledTextColor.b, 0.2) : "transparent")
                                                  
        HoverHandler {
            id: hoverHandler
        }
    }
    
    contentItem: RowLayout {
        spacing: Kirigami.Units.gridUnit
        height: Kirigami.Units.gridUnit * 3
        
        Kirigami.Icon {
            Layout.leftMargin: Kirigami.Units.gridUnit
            implicitHeight: Kirigami.Units.iconSizes.smallMedium
            implicitWidth: Kirigami.Units.iconSizes.smallMedium
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            visible: button.icon.name
            source: button.icon.name
        }
        
        Controls.Label {
            id: label
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            text: button.text
            opacity: button.display !== Controls.AbstractButton.TextOnly ? 1 : 0
            
            Behavior on opacity {
                NumberAnimation {
                    duration: Kirigami.Units.longDuration 
                    easing.type: Easing.InOutQuad
                } 
            }
        }
        
        Item { Layout.fillWidth: true }
    }
}
 
