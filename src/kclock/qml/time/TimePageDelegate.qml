/*
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2023 Nate Graham <nate@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

Control {
    id: root

    property bool editMode
    required property string city
    required property string relativeTime
    required property string timeString

    signal deleteRequested()

    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing
    leftPadding: Kirigami.Units.gridUnit
    rightPadding: Kirigami.Units.gridUnit

    contentItem: RowLayout {
        spacing: Kirigami.Units.largeSpacing

        ColumnLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            Label {
                Layout.fillWidth: true
                text: root.city
                font.bold: true
            }
            Label {
                Layout.fillWidth: true
                text: root.relativeTime
                font: Kirigami.Theme.smallFont
            }
        }

        Label {
            text: root.timeString
            font.weight: Font.Bold
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.2
            opacity: 0.7
            color: Kirigami.Theme.textColor
        }

        ToolButton {
            icon.name: "delete"
            text: i18nc("@action:button", "Delete")
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
