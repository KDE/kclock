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
        if (!page) {
            return;
        }
        page.parent = pageParent;
        page.anchors.fill = pageParent;
        page.visible = true;
    }

    ColumnLayout {
        spacing: 0
        anchors.fill: parent

        PageHeader {
            // This doesn't work because it assumes ColumnView is parent:
            // visible: page.globalToolBarStyle !== Kirigami.ApplicationHeaderStyle.None

            // HACK: we don't have a way of knowing whether to display the title at the moment,
            //       so setting `titleDelegate` to null on the page will have to do.
            visible: root.page ? root.page.titleDelegate : false

            titleDelegate: root.page ? root.page.titleDelegate : null
            actions: root.page ? root.page.actions : []
            showBackButton: root.showBackButton
        }

        Item {
            id: pageParent
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}