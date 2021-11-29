/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1 as QQC2
import org.kde.kirigami 2.11 as Kirigami

Kirigami.OverlayDrawer {
    id: drawer
    modal: false
    width: 200
    height: appwindow.height
    
    Kirigami.Theme.colorSet: Kirigami.Theme.Window
    Kirigami.Theme.inherit: false
    
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    
    contentItem: ColumnLayout {
        spacing: 0
        
        QQC2.ToolBar {
            Layout.fillWidth: true
            implicitHeight: applicationWindow().pageStack.globalToolBar.preferredHeight

            Item {
                anchors.fill: parent
                Kirigami.Heading {
                    level: 1
                    text: i18n("Clock")
                    anchors.left: parent.left
                    anchors.leftMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
        
        ColumnLayout {
            id: column
            spacing: 0
            Layout.margins: Kirigami.Units.smallSpacing
            
            SidebarButton {
                text: i18n("Time")
                icon.name: "clock"
                Layout.fillWidth: true
                Layout.minimumHeight: Kirigami.Units.gridUnit * 2
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                checked: pageStack.currentItem === page
                property var page: appwindow.getPage("Time")
                onClicked: {
                    if (appwindow.pageStack.currentItem !== page) {
                        appwindow.switchToPage(page, 0);
                    } else {
                        checked = Qt.binding(function() { return appwindow.pageStack.currentItem === page; });
                    }
                }
            }
            
            SidebarButton {
                text: i18n("Timers")
                icon.name: "player-time"
                Layout.fillWidth: true
                Layout.minimumHeight: Kirigami.Units.gridUnit * 2
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                checked: pageStack.currentItem === page
                property var page: appwindow.getPage("Timers")
                onClicked: {
                    if (appwindow.pageStack.currentItem !== page) {
                        appwindow.switchToPage(page, 0);
                    } else {
                        checked = Qt.binding(function() { return appwindow.pageStack.currentItem === page; });
                    }
                }
            }
            
            SidebarButton {
                text: i18n("Stopwatch")
                icon.name: "chronometer"
                Layout.fillWidth: true
                Layout.minimumHeight: Kirigami.Units.gridUnit * 2
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                checked: pageStack.currentItem === page
                property var page: appwindow.getPage("Stopwatch")
                onClicked: {
                    if (appwindow.pageStack.currentItem !== page) {
                        appwindow.switchToPage(page, 0);
                    } else {
                        checked = Qt.binding(function() { return appwindow.pageStack.currentItem === page; });
                    }
                }
            }
            
            SidebarButton {
                text: i18n("Alarms")
                icon.name: "notifications"
                Layout.fillWidth: true
                Layout.minimumHeight: Kirigami.Units.gridUnit * 2
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                checked: pageStack.currentItem === page
                property var page: appwindow.getPage("Alarms")
                onClicked: {
                    if (appwindow.pageStack.currentItem !== page) {
                        appwindow.switchToPage(page, 0);
                    } else {
                        checked = Qt.binding(function() { return appwindow.pageStack.currentItem === page; });
                    }
                }
            }
            
            Item { Layout.fillHeight: true }
            Kirigami.Separator { 
                Layout.fillWidth: true 
                Layout.margins: Kirigami.Units.smallSpacing
            }
            
            SidebarButton {
                text: i18n("Settings")
                icon.name: "settings-configure"
                Layout.fillWidth: true
                Layout.minimumHeight: Kirigami.Units.gridUnit * 2
                checked: pageStack.currentItem === page
                property var page: appwindow.getPage("Settings")
                onClicked: {
                    if (appwindow.pageStack.currentItem !== page) {
                        appwindow.switchToPage(page, 0);
                    } else {
                        checked = Qt.binding(function() { return appwindow.pageStack.currentItem === page; });
                    }
                }
            }
        }
    }
}
