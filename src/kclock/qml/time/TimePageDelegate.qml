/*
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2023 Nate Graham <nate@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.20 as Kirigami

import kclock 1.0

Kirigami.BasicListItem {
    id: root
    property bool editMode

    signal deleteRequested()
    
    leftPadding: Kirigami.Units.largeSpacing * 2
    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing
    rightPadding: Kirigami.Units.largeSpacing * 2
    
    label: model.city
    subtitle: model.relativeTime

    bold: true

    // Don't need a hover effect for the background, since these items don't
    // do anything when tapped or clicked
    activeBackgroundColor: "transparent"
    activeTextColor: Kirigami.Theme.textColor

    trailing: RowLayout {
        spacing: Kirigami.Units.smallSpacing

        Label {
            text: model.timeString
        }

        ToolButton {
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
