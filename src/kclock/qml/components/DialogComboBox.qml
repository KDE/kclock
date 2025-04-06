/*
 * Copyright 2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

Button {
    id: control

    property alias title: dialog.title
    property alias dialogDelegate: repeater.delegate
    property alias model: repeater.model

    onClicked: dialog.open()

    Kirigami.Dialog {
        id: dialog

        showCloseButton: false

        ColumnLayout {
            spacing: 0

            Kirigami.Theme.inherit: false
            Kirigami.Theme.colorSet: Kirigami.Theme.View

            Repeater {
                id: repeater
            }
        }
    }

    Kirigami.Icon {
        source: "go-down-symbolic"

        implicitWidth: Kirigami.Units.iconSizes.small
        implicitHeight: Kirigami.Units.iconSizes.small

        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
            rightMargin: Kirigami.Units.smallSpacing
        }
    }
}

