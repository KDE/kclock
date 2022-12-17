/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami

Kirigami.OverlayDrawer {
    id: drawer
    modal: false
    width: 100
    height: applicationWindow().height
    
    Kirigami.Theme.colorSet: Kirigami.Theme.Window
    Kirigami.Theme.inherit: false
    
    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    
    contentItem: ColumnLayout {
        spacing: 0
        
        Kirigami.AbstractApplicationHeader {
            Layout.fillWidth: true
        }
        
        QQC2.ScrollView {
            id: scrollView
            Layout.fillWidth: true
            Layout.fillHeight: true

            QQC2.ScrollBar.vertical.policy: QQC2.ScrollBar.AlwaysOff
            QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff
            contentWidth: -1 // disable horizontal scroll

            ColumnLayout {
                id: column
                width: scrollView.width
                spacing: 0

                Kirigami.NavigationTabButton {
                    property var page: applicationWindow().getPage("Time")

                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

                    text: i18n("Time")
                    icon.name: "clock"
                    checked: pageStack.currentItem === page
                    onClicked: {
                        if (applicationWindow().pageStack.currentItem !== page) {
                            applicationWindow().switchToPage(page, 0);
                        } else {
                            checked = Qt.binding(function() { return applicationWindow().pageStack.currentItem === page; });
                        }
                    }
                }

                Kirigami.NavigationTabButton {
                    property var page: applicationWindow().getPage("Timers")

                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

                    text: i18n("Timers")
                    icon.name: "player-time"
                    checked: pageStack.currentItem === page
                    onClicked: {
                        if (applicationWindow().pageStack.currentItem !== page) {
                            applicationWindow().switchToPage(page, 0);
                        } else {
                            checked = Qt.binding(function() { return applicationWindow().pageStack.currentItem === page; });
                        }
                    }
                }

                Kirigami.NavigationTabButton {
                    property var page: applicationWindow().getPage("Stopwatch")

                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

                    text: i18n("Stopwatch")
                    icon.name: "chronometer"
                    checked: pageStack.currentItem === page
                    onClicked: {
                        if (applicationWindow().pageStack.currentItem !== page) {
                            applicationWindow().switchToPage(page, 0);
                        } else {
                            checked = Qt.binding(function() { return applicationWindow().pageStack.currentItem === page; });
                        }
                    }
                }

                Kirigami.NavigationTabButton {
                    property var page: applicationWindow().getPage("Alarms")

                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

                    text: i18n("Alarms")
                    icon.name: "notifications"
                    checked: pageStack.currentItem === page
                    onClicked: {
                        if (applicationWindow().pageStack.currentItem !== page) {
                            applicationWindow().switchToPage(page, 0);
                        } else {
                            checked = Qt.binding(function() { return applicationWindow().pageStack.currentItem === page; });
                        }
                    }
                }
            }
        }
            
        Kirigami.Separator {
            Layout.fillWidth: true
            Layout.rightMargin: Kirigami.Units.smallSpacing
            Layout.leftMargin: Kirigami.Units.smallSpacing
        }

        Kirigami.NavigationTabButton {
            property var page: applicationWindow().getPage("Settings")

            Layout.fillWidth: true
            width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

            text: i18n("Settings")
            icon.name: "settings-configure"
            checked: pageStack.currentItem === page
            onClicked: {
                if (applicationWindow().pageStack.currentItem !== page) {
                    applicationWindow().switchToPage(page, 0);
                } else {
                    checked = Qt.binding(function() { return applicationWindow().pageStack.currentItem === page; });
                }
            }
        }
    }
}
