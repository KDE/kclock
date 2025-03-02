/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Templates as T
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

import "components"

Kirigami.AbstractApplicationWindow {
    id: root

    // needs to work with 360x720 (+ panel heights)
    minimumWidth: 300
    minimumHeight: minimumWidth + 1
    width: Kirigami.Settings.isMobile ? 360 : 550
    height: Kirigami.Settings.isMobile ? 720 : 500

    title: i18n("Clock")

    contextDrawer: Kirigami.ContextDrawer {}

    property bool isWidescreen: root.width >= 500
    onIsWidescreenChanged: changeNav(isWidescreen);

    pageStack: PageStack {}

    Kirigami.PagePool {
        id: pagePool
    }

    Component.onCompleted: {
        // initial page and nav type
        switchToPage(getPage("Time"), 1);
        changeNav(isWidescreen);
    }

    function switchToPage(page, depth) {
        // pop pages above depth
        while (pageStack.depth > depth) {
            if (pageStack.depth == 1) {
                pageStack.clear();
            } else {
                pageStack.pop();
            }
        }

        pageStack.push(page);
    }

    function getPage(name) {
        switch (name) {
            case "Time": return pagePool.loadPage("qrc:/qml/time/TimePage.qml");
            case "Timers": return pagePool.loadPage("qrc:/qml/timer/TimerListPage.qml");
            case "Stopwatch": return pagePool.loadPage("qrc:/qml/stopwatch/StopwatchPage.qml");
            case "Alarms": return pagePool.loadPage("qrc:/qml/alarm/AlarmListPage.qml");
            case "Settings": return pagePool.loadPage("qrc:/qml/settings/SettingsPage.qml");
            case "About": return pagePool.loadPage("qrc:/qml/components/AboutPage.qml");
        }
    }

    // switch between bottom toolbar and sidebar
    function changeNav(toWidescreen) {
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

            let bottomToolbar = Qt.createComponent("qrc:/qml/components/BottomToolbar.qml")
            footer = bottomToolbar.createObject(root);
        }
    }

    Loader {
        id: sidebarLoader
        source: "qrc:/qml/components/Sidebar.qml"
        active: false
    }
}
