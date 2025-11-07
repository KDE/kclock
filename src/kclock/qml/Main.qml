/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import org.kde.kirigami as Kirigami
import org.kde.kclock as KClock

Kirigami.ApplicationWindow {
    id: root

    required property string initialPage

    // needs to work with 360x720 (+ panel heights)
    minimumWidth: 300
    minimumHeight: minimumWidth + 1
    width: Kirigami.Settings.isMobile ? 360 : 550
    height: Kirigami.Settings.isMobile ? 720 : 500

    title: {
        const currentPage = pageStack.currentItem
        if (currentPage) {
            if (!KClock.WindowExposure.exposed) {
                if (currentPage.hiddenTitle) {
                    return currentPage.hiddenTitle;
                }
            }
            return currentPage.title;
        }
        return "";
    }

    contextDrawer: Kirigami.ContextDrawer {}

    pageStack {
        globalToolBar {
            canContainHandles: true
            style: Kirigami.ApplicationHeaderStyle.ToolBar
            showNavigationButtons: Kirigami.ApplicationHeaderStyle.ShowBackButton;
        }
        popHiddenPages: true

        columnView.columnResizeMode: Kirigami.ColumnView.SingleColumn
    }

    property bool isWidescreen: root.width >= 500
    onIsWidescreenChanged: changeNav(isWidescreen);

    property Kirigami.Action settingsAction: Kirigami.Action {
        text: i18n("Settings")
        icon.name: "settings-configure"
        checked: root.pageStack.currentItem?.objectName === "Settings"
        shortcut: StandardKey.Preferences
        onTriggered: {
            if (root.pageStack.currentItem?.objectName !== "Settings") {
                root.switchToPage(root.getPage("Settings"), 0);
            }
        }
    }

    Kirigami.PagePool {
        id: pagePool
    }

    Component.onCompleted: {
        // initial page and nav type
        let page = getPage(root.initialPage);
        if (!page) {
            console.warn("Unknown initial page", root.initialPage);
            page = getPage("Time");
        }
        switchToPage(page, 1);
        changeNav(isWidescreen);
    }

    function switchToPage(page, depth) {
        // pop pages above depth
        while (pageStack.depth > depth) pageStack.pop();
        while (pageStack.layers.depth > 1) pageStack.layers.pop();

        // page switch animation
        yAnim.target = page;
        yAnim.properties = "yTranslate";
        anim.target = page;
        anim.properties = "contentItem.opacity";
        if (page.header) {
            anim.properties += ",header.opacity";
        }
        yAnim.restart();
        anim.restart();

        return pageStack.push(page);
    }

    function getPage(name) {
        switch (name) {
            case "Time": return pagePool.loadPage(Qt.resolvedUrl("time/TimePage.qml"));
            case "Timers": return pagePool.loadPage(Qt.resolvedUrl("timer/TimerListPage.qml"));
            case "Stopwatch": return pagePool.loadPage(Qt.resolvedUrl("stopwatch/StopwatchPage.qml"));
            case "Alarms": return pagePool.loadPage(Qt.resolvedUrl("alarm/AlarmListPage.qml"));
            case "Settings": return pagePool.loadPage(Qt.resolvedUrl("settings/SettingsPage.qml"));
            case "About": return pagePool.loadPage(Qt.resolvedUrl("components/AboutPage.qml"));
        }
    }

    // switch between bottom toolbar and sidebar
    function changeNav(toWidescreen : bool) : void {
        if (toWidescreen) {
            if (footer !== null) {
                footer.destroy();
                footer = null;
            }
            sidebarLoader.active = true;
            root.globalDrawer = sidebarLoader.item;
        } else {
            sidebarLoader.active = false;
            globalDrawer = null;

            let bottomToolbar = Qt.createComponent(Qt.resolvedUrl("components/BottomToolbar.qml"));
            footer = bottomToolbar.createObject(root);
        }
    }

    Connections {
        target: KClock.PipHandler
        function onRestoreClicked(pipPage) {
            if (!root.visible) {
                root.show();
            }
            // FIXME Doesn't work, just blinks in taskbar.
            root.requestActivate();

            const currentPage = pageStack.currentItem?.objectName;

            if (pipPage.objectName === "StopwatchPip") {
                if (currentPage !== "Stopwatch") {
                    const page = root.getPage("Stopwatch");
                    switchToPage(page, 0);
                }
            } else if (pipPage.objectName === "TimerPip") {
                if (currentPage === "Timer") {
                    pageStack.currentItem.timer = pipPage.timer;
                } else if (currentPage === "Timers") {
                    pageStack.currentItem.openTimer(pipPage.timer);
                } else {
                    const timersPage = switchToPage(root.getPage("Timers"), 0);
                    timersPage.openTimer(pipPage.timer);
                }
            }
        }
    }

    // page switch animation
    NumberAnimation {
        id: anim
        from: 0
        to: 1
        duration: Kirigami.Units.veryLongDuration
        easing.type: Easing.InOutQuad
    }
    NumberAnimation {
        id: yAnim
        from: Kirigami.Units.gridUnit * 2
        to: 0
        duration: Kirigami.Units.longDuration * 3
        easing.type: Easing.OutExpo
    }

    Loader {
        id: sidebarLoader
        source: Qt.resolvedUrl("components/Sidebar.qml")
        active: false
    }
}
