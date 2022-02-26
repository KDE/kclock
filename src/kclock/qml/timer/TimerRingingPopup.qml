/*
 * Copyright 2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.2

import org.kde.kirigami 2.19 as Kirigami

import "../components"
import "../components/formatUtil.js" as FormatUtil
import kclock 1.0

Kirigami.Dialog {
    id: root
    
    property Timer timer
    
    readonly property string length: timer ? timer.lengthPretty : ""
    readonly property string label: timer ? timer.label : ""

    padding: Kirigami.Units.gridUnit * 3
    
    title: i18n("Timer has finished")
    showCloseButton: false
    closePolicy: Popup.NoAutoClose
    
    ColumnLayout {
        width: implicitWidth
        implicitWidth: Kirigami.Units.gridUnit * 20
        Label {
            Layout.alignment: Qt.AlignHCenter
            font.weight: Font.Light
            font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 2)
            wrapMode: Text.Wrap
            text: root.length
        }
        
        Label {
            Layout.alignment: Qt.AlignHCenter
            font.weight: Font.Bold
            font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 1.25)
            text: root.label ? i18n("%1 has completed.", root.label) : i18n("The timer has completed.")
            wrapMode: Text.Wrap
        }
    }
    
    standardButtons: Kirigami.Dialog.NoButton
    flatFooterButtons: true
    
    customFooterActions: [
        Kirigami.Action {
            text: i18n("Dismiss")
            onTriggered: {
                timer.dismiss();
            }
        }
    ]
}

