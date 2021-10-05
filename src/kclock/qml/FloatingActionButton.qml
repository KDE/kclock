/*
 * SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.12
import org.kde.kirigami 2.15 as Kirigami

Item {
    id: root
    property string iconName
    signal clicked()
    
    RectangularGlow {
        anchors.fill: button
        anchors.topMargin: 5
        cornerRadius: button.radius * 2
        cached: true
        glowRadius: 4
        spread: 0.8
        color: Qt.darker(Kirigami.Theme.backgroundColor, 1.2)
    }

    Rectangle {
        id: button
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Kirigami.Units.largeSpacing
        
        implicitWidth: Kirigami.Units.gridUnit * 3
        implicitHeight: Kirigami.Units.gridUnit * 3
        radius: width / 2
        color: Kirigami.Theme.highlightColor
        
        Controls.AbstractButton {
            anchors.fill: parent
            onPressedChanged: {
                if (pressed) {
                    parent.color = Qt.darker(Kirigami.Theme.highlightColor, 1.1)
                } else {
                    parent.color = Kirigami.Theme.highlightColor
                }
            }
            onClicked: root.clicked()
        }
        
        Kirigami.Icon {
            anchors.centerIn: parent
            source: root.iconName
            Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
            Kirigami.Theme.inherit: false
            isMask: true
            implicitWidth: Kirigami.Units.iconSizes.smallMedium
            implicitHeight: Kirigami.Units.iconSizes.smallMedium
        }
    }
}
