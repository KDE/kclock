/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.12 as Kirigami
//import QtGraphicalEffects 1.12

ToolBar {
    id: toolbarRoot
    property double iconSize: Math.round(Kirigami.Units.gridUnit * 1.5)
    property double shrinkIconSize: Math.round(Kirigami.Units.gridUnit * 1.1)
    property double fontSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 0.8)
    property double shrinkFontSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 0.7)

    background: Rectangle {
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false
        color: Kirigami.Theme.backgroundColor
        anchors.fill: parent
        
        layer.enabled: true
    }
    
    RowLayout {
        anchors.fill: parent
        spacing: 0
        BottomToolBarButton {
            buttonName: i18n("Time")
            buttonIcon: "clock"
        }
        BottomToolBarButton {
            buttonName: i18n("Timers")
            buttonIcon: "player-time"
        }
        BottomToolBarButton {
            buttonName: i18n("Stopwatch")
            buttonIcon: "chronometer"
        }
        BottomToolBarButton {
            buttonName: i18n("Alarms")
            buttonIcon: "notifications"
        }
        BottomToolBarButton {
            buttonName: i18n("Settings")
            buttonIcon: "settings-configure"
        }
    }
} 
