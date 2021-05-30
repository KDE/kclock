/*
 * Copyright 2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.15 as Kirigami
import kclock 1.0
        
Loader {
    id: loader
    sourceComponent: Kirigami.Settings.isMobile ? mobileComponent : desktopComponent
    
    Component {
        id: mobileComponent
        Kirigami.OverlayDrawer {
            height: contents.implicitHeight + Kirigami.Units.largeSpacing
            width: timePage.width
            edge: Qt.BottomEdge
            parent: applicationWindow().overlay
            
            ColumnLayout {
                id: contents
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 0
                
                Kirigami.Icon {
                    Layout.margins: Kirigami.Units.smallSpacing
                    source: "arrow-down"
                    implicitWidth: Kirigami.Units.gridUnit
                    implicitHeight: Kirigami.Units.gridUnit
                    Layout.alignment: Qt.AlignHCenter
                }
                
                Kirigami.Heading {
                    level: 3
                    text: i18n("<b>Timezones</b>")
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: Kirigami.Units.smallSpacing
                }
                
                TimeZoneSelect {
                    id: timerForm
                    Layout.leftMargin: Kirigami.Units.smallSpacing
                    Layout.rightMargin: Kirigami.Units.smallSpacing
                    Layout.fillWidth: true
                }
            }
        }
    }
    
    Component {
        id: desktopComponent
        Kirigami.OverlaySheet {
            parent: applicationWindow().overlay
            header: Kirigami.Heading {
                level: 2
                text: i18n("Timezones")
            }
            
            contentItem: TimeZoneSelect {
                Layout.fillWidth: true
            }
        }
    }
} 
