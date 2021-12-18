/*
 * Copyright 2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.3

import org.kde.kirigami 2.19 as Kirigami

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
            Kirigami.Theme.inherit: false
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            spacing: 0
            
            Repeater {
                id: repeater
            }
        }
    }
    
    Kirigami.Icon {
        source: "go-down-symbolic"
        implicitWidth: Kirigami.Units.iconSizes.small
        implicitHeight: Kirigami.Units.iconSizes.small
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: Kirigami.Units.smallSpacing
    }
}

