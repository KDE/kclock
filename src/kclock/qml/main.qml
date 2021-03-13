/*
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *           2020 Devin Lin <espidev@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.11 as Kirigami

Kirigami.ApplicationWindow
{
    id: appwindow
    width: Kirigami.Settings.isMobile ? 400 : 650
    height: Kirigami.Settings.isMobile ? 650 : 500

    title: i18n("Clock")

    Kirigami.PagePool {
        id: pagePool
    }
    
    pageStack.initialPage: pagePool.loadPage("TimePage.qml")
    
    function switchToPage(page, depth) {
        while (pageStack.depth > depth) pageStack.pop()
        while (pageStack.layers.depth > 1) pageStack.layers.pop()
        pageStack.push(page);
    }
    
    function getPage(name) {
        switch (name) {
            case "Time": return pagePool.loadPage("qrc:/qml/TimePage.qml");
            case "Timer": return pagePool.loadPage("qrc:/qml/TimerListPage.qml");
            case "Stopwatch": return pagePool.loadPage("qrc:/qml/StopwatchPage.qml");
            case "Alarm": return pagePool.loadPage("qrc:/qml/AlarmListPage.qml");
            case "Settings": return pagePool.loadPage("qrc:/qml/SettingsPage.qml");
            case "About": return pagePool.loadPage("qrc:/qml/AboutPage.qml");
        }
    }
    
    property bool isWidescreen: appwindow.width >= appwindow.height
    onIsWidescreenChanged: changeNav(!isWidescreen);
    
    // switch between bottom toolbar and sidebar
    function changeNav(toNarrow) {
        if (toNarrow) {
            sidebarLoader.active = false;
            globalDrawer = null;
            
            let bottomToolbar = Qt.createComponent("qrc:/qml/BottomToolbar.qml")
            footer = bottomToolbar.createObject(appwindow);
        } else {
            if (footer !== null) {
                footer.destroy();
                footer = null;
            }
            sidebarLoader.active = true;
            globalDrawer = sidebarLoader.item;
        }
    }
    Component.onCompleted: changeNav(!isWidescreen)
    
    Loader {
        id: sidebarLoader
        source: "qrc:/qml/Sidebar.qml"
        active: false
    }
    
    // clock fonts
    FontLoader {
        id: clockFont
        source: "/assets/NotoSans-Light.ttf"
    }
}
