/*
 * Copyright 2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.19 as Kirigami
import kclock 1.0
        
Loader {
    id: loader
    sourceComponent: Kirigami.Settings.isMobile ? mobileComponent : desktopComponent
    
    Component {
        id: mobileComponent
        Kirigami.OverlayDrawer {
            id: drawer
            
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
                    text: i18n("<b>Add Location</b>")
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: Kirigami.Units.smallSpacing
                }
                
                AddLocation {
                    id: timerForm
                    Layout.leftMargin: Kirigami.Units.smallSpacing
                    Layout.rightMargin: Kirigami.Units.smallSpacing
                    Layout.fillWidth: true
                    
                    onCloseRequested: drawer.close()
                }
            }
        }
    }
    
    Component {
        id: desktopComponent
        Kirigami.Dialog {
            id: dialog
            
            standardButtons: Kirigami.Dialog.NoButton
            parent: applicationWindow().overlay
            title: i18n("Add Location")
            preferredWidth: Kirigami.Units.gridUnit * 20
            padding: Kirigami.Units.largeSpacing
            
            AddLocation {
                onCloseRequested: dialog.close()
            }
        }
    }
} 
