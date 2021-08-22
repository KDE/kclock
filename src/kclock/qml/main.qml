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

Kirigami.ApplicationWindow
{
    id: appwindow
    width: 400
    height: 650
    property string currentPage: "Time"
    title: i18n("Clock")

    Kirigami.PagePool {
        id: pagePool
    }
    footer: BottomToolbar {}
    pageStack.initialPage: TimePage {}
    
    // page switch animation
//    NumberAnimation {
//        id: anim
//        from: 0
//        to: 1
//        duration: Kirigami.Units.longDuration * 2
//        easing.type: Easing.InOutQuad
//    }
//    NumberAnimation {
//        id: yAnim
//        from: Kirigami.Units.gridUnit * 3
//        to: 0
//        duration: Kirigami.Units.longDuration * 3
//        easing.type: Easing.OutQuint
//    }
    
    function switchToPage(page, depth) {
        while (pageStack.depth > depth) pageStack.pop();
        while (pageStack.layers.depth > 1) pageStack.layers.pop();
        
        // page switch animation
//        yAnim.target = page;
//        yAnim.properties = "yTranslate";
//        anim.target = page;
//        anim.properties = "mainItem.opacity";
//        if (page.header) {
//            anim.properties += ",header.opacity";
//        }
//        yAnim.restart();
//        anim.restart();
        pageStack.push(page);
    }
    
    function getPage(name) {
        switch (name) {
            case "Time": return pagePool.loadPage("qrc:/qml/TimePage.qml");
            case "Timers": return pagePool.loadPage("qrc:/qml/TimerListPage.qml");
            case "Stopwatch": return pagePool.loadPage("qrc:/qml/StopwatchPage.qml");
            case "Alarms": return pagePool.loadPage("qrc:/qml/AlarmListPage.qml");
            case "Settings": return pagePool.loadPage("qrc:/qml/SettingsPage.qml");
            case "About": return pagePool.loadPage("qrc:/qml/AboutPage.qml");
        }
    }
    
    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
        handle.anchors.bottomMargin: appwindow.footer.height + Kirigami.Units.largeSpacing
    }
}
