// SPDX-FileCopyrightText: 2021-2022 Devin Lin <espidev@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.12
import QtQuick.Controls 2.15 as Controls
import QtQuick.Templates 2.15 as T
import QtQuick.Layouts 1.2

import org.kde.kirigami 2.15 as Kirigami

/**
 * @brief Floating button that can be used on pages to trigger actions.
 * 
 * It is automatically anchored to be horizontally centered in the parent component, and towards the bottom.
 * 
 * These anchors can be overridden.
 * 
 * Example usage:
 * @code{.qml}
 * import QtQuick 2.15
 * import QtQuick.Layouts 1.15
 * import org.kde.kirigami 2.21 as Kirigami
 * 
 * Kirigami.ScrollablePage {
 *     title: i18n("Page with ListView")
 * 
 *     ListView {
 *         model: 50
 *         delegate: Kirigami.BasicListItem {
 *             text: modelData
 *         }
 * 
 *         Kirigami.FloatingActionButton {
 *             icon.name: "list-add"
 *             onClicked: {
 *                 // behaviour when clicked
 *             }
 *         }
 *     }
 * }
 * @endcode
 */
T.RoundButton {
    id: root
    width: implicitWidth
    height: implicitHeight
    implicitWidth: Kirigami.Units.gridUnit * 3
    implicitHeight: Kirigami.Units.gridUnit * 3
    
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.bottomMargin: Kirigami.Units.largeSpacing
    
    background: Kirigami.ShadowedRectangle {
        anchors.fill: parent
        radius: root.radius
        color: root.pressed ? Qt.darker(Kirigami.Theme.highlightColor, 1.2) : Kirigami.Theme.highlightColor
        
        property color shadowColor: Qt.darker(Kirigami.Theme.backgroundColor, 1.5)
        shadow.color: Qt.rgba(shadowColor.r, shadowColor.g, shadowColor.b, 0.5)
        shadow.size: 7
        shadow.yOffset: 2
    }
    
    contentItem: Item {
        Kirigami.Icon {
            anchors.centerIn: parent
            source: root.icon.name
            Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
            Kirigami.Theme.inherit: false
            isMask: true
            implicitWidth: Kirigami.Units.iconSizes.smallMedium
            implicitHeight: Kirigami.Units.iconSizes.smallMedium
        }
    }
}
