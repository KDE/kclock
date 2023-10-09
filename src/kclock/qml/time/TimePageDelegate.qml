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
import org.kde.kirigamiaddons.delegates as Delegates

import kclock

Delegates.RoundedItemDelegate {
    id: root

    property bool editMode
    required property string city
    required property string relativeTime
    required property string timeString

    signal deleteRequested()

    text: city

    contentItem: RowLayout {
        spacing: Kirigami.Units.smallSpacing

        Delegates.SubtitleContentItem {
            itemDelegate: root
            subtitle: root.relativeTime
            Layout.fillWidth: true
        }

        Label {
            text: root.timeString
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
