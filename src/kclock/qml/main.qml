/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.11 as Kirigami

Kirigami.ApplicationWindow {
    id: root
    
    // needs to work with 360x720 (+ panel heights)
    minimumWidth: 300
    minimumHeight: minimumWidth + 1
    width: Kirigami.Settings.isMobile ? 360 : 650
    height: Kirigami.Settings.isMobile ? 720 : 500

    title: i18n("Clock")
    
    contextDrawer: Kirigami.ContextDrawer {}
    
    pageStack.globalToolBar.canContainHandles: true
    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.ToolBar

    property bool isWidescreen: root.width >= root.height
    onIsWidescreenChanged: changeNav(isWidescreen);
    
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
        while (pageStack.depth > depth) pageStack.pop();
        while (pageStack.layers.depth > 1) pageStack.layers.pop();
        
        // page switch animation
        yAnim.target = page;
        yAnim.properties = "yTranslate";
        anim.target = page;
        anim.properties = "mainItem.opacity";
        if (page.header) {
            anim.properties += ",header.opacity";
        }
        yAnim.restart();
        anim.restart();
        
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
            globalDrawer = sidebarLoader.item;
        } else {
            sidebarLoader.active = false;
            globalDrawer = null;
            
            let bottomToolbar = Qt.createComponent("qrc:/qml/components/BottomToolbar.qml")
            footer = bottomToolbar.createObject(root);
        }
    }
    
    // page switch animation
    NumberAnimation {
        id: anim
        from: 0
        to: 1
        duration: Kirigami.Units.longDuration * 2
        easing.type: Easing.InOutQuad
    }
    NumberAnimation {
        id: yAnim
        from: Kirigami.Units.gridUnit * 3
        to: 0
        duration: Kirigami.Units.longDuration * 3
        easing.type: Easing.OutQuint
    }
    
    Loader {
        id: sidebarLoader
        source: "qrc:/qml/components/Sidebar.qml"
        active: false
    }
}
