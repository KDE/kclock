/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami

Kirigami.OverlayDrawer {
    id: drawer
    modal: false
    width: 100
    height: applicationWindow().height

    edge: Qt.application.layoutDirection === Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
    parent: QQC2.Overlay.overlay
    x: 0

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
                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

                    text: i18n("Time")
                    icon.name: "clock"
                    checked: pageStack.currentItem?.objectName === "Time"
                    onClicked: {
                        if (pageStack.currentItem?.objectName !== "Time") {
                            const page = applicationWindow().getPage("Time");
                            applicationWindow().switchToPage(page, 0);
                        }
                    }
                }

                Kirigami.NavigationTabButton {
                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

                    text: i18n("Timers")
                    icon.name: "player-time"
                    checked: pageStack.currentItem?.objectName === "Timers"
                    onClicked: {
                        if (pageStack.currentItem?.objectName !== "Timers") {
                            const page = applicationWindow().getPage("Timers");
                            applicationWindow().switchToPage(page, 0);
                        }
                    }
                }

                Kirigami.NavigationTabButton {
                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

                    text: i18n("Stopwatch")
                    icon.name: "chronometer"
                    checked: pageStack.currentItem?.objectName === "Stopwatch"
                    onClicked: {
                        if (pageStack.currentItem?.objectName !== "Stopwatch") {
                            const page = applicationWindow().getPage("Stopwatch");
                            applicationWindow().switchToPage(page, 0);
                        }
                    }
                }

                Kirigami.NavigationTabButton {
                    Layout.fillWidth: true
                    width: column.width - column.Layout.leftMargin - column.Layout.rightMargin

                    text: i18n("Alarms")
                    icon.name: "notifications"
                    checked: pageStack.currentItem?.objectName === "Alarms"
                    onClicked: {
                        if (pageStack.currentItem?.objectName !== "Alarms") {
                            const page = applicationWindow().getPage("Alarms");
                            applicationWindow().switchToPage(page, 0);
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
            Layout.fillWidth: true
            action: applicationWindow().settingsAction
        }
    }
}
