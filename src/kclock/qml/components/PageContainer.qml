/*
 * Copyright 2025 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Templates as T
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Item {
    id: root

    property Kirigami.Page page
    property bool showBackButton

    onPageChanged: {
        page.parent = pageParent;
        page.anchors.fill = pageParent;
        page.visible = true;
    }

    ColumnLayout {
        spacing: 0
        anchors.fill: parent

        PageHeader {
            // visible: page.globalToolBarStyle !== Kirigami.ApplicationHeaderStyle.None
            title: root.page.title // TODO titleDelegate
            actions: root.page.actions
            showBackButton: root.showBackButton
        }

        Item {
            id: pageParent
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}