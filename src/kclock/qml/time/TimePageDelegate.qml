/*
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.12 as Kirigami

import "../components"

import kclock 1.0

ListDelegate {
    id: root
    property bool editMode
    
    signal deleteRequested()
    
    leftPadding: Kirigami.Units.largeSpacing * 2
    rightPadding: Kirigami.Units.largeSpacing * 2
    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing
    
    contentItem: RowLayout {
        ColumnLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing
            Label {
                text: model.city
                font.weight: Font.Bold
            }
            Label {
                text: model.relativeTime
            }
        }
        
        Item { Layout.fillWidth: true }
        
        Kirigami.Heading {
            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            id: timeText
            level: 2
            text: model.timeString
        }
        
        ToolButton {
            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            icon.name: "delete"
            text: i18n("Delete")
            onClicked: root.deleteRequested()
            visible: root.editMode
            display: AbstractButton.IconOnly
            
            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.timeout: 5000
            ToolTip.visible: Kirigami.Settings.tabletMode ? pressed : hovered
            ToolTip.text: text
        }
    }
}
