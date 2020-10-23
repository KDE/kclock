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
    height: 500

    title: i18n("Clock")

    pageStack.initialPage: timePage
    
    function switchToPage(page, depth) {
        while (pageStack.depth > depth) pageStack.pop()
        pageStack.push(page)
    }
    
    function getPage(name) {
        switch (name) {
            case "Time": return timePage;
            case "Timer": return timerListPage;
            case "Stopwatch": return stopwatchPage;
            case "Alarm": return alarmPage;
            case "Settings": return settingsPage;
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
        id: clockFont;
        source: "/assets/RedHatText-Regular.ttf"
    }
    
    TimePage {
        id: timePage
        objectName: "time"
    }
    TimerListPage {
        id: timerListPage
        objectName: "timer"
        visible: false
    }
    StopwatchPage {
        id: stopwatchPage
        objectName: "stopwatch"
        visible: false
    }
    AlarmPage {
        id: alarmPage
        objectName: "alarm"
        visible: false
    }
    SettingsPage {
        id: settingsPage
        objectName: "settings"
        visible: false
    }
        
    Kirigami.AboutPage {
        id: aboutPage
        visible: false
        aboutData: {
            "displayName": "Clock",
            "productName": "kirigami/clock",
            "componentName": "kclock",
            "shortDescription": "A mobile friendly clock app built with Kirigami.",
            "homepage": "",
            "bugAddress": "",
            "version": "0.2",
            "otherText": "",
            "copyrightStatement": "© 2020 Plasma Development Team",
            "desktopFileName": "org.kde.kclock",
            "authors": [
                        {
                            "name": "Devin Lin",
                            "emailAddress": "espidev@gmail.com",
                            "webAddress": "https://espi.dev",
                        },
                        {
                            "name": "Han Young",
                            "emailAddress": "hanyoung@protonmail.com",
                            "webAddress": "https://han-y.gitlab.io",
                        },

                    ],
            "licenses" : [
                        {
                            "name" : "GPL v2",
                            "text" : "long, boring, license text",
                            "spdx" : "GPL-2.0",
                        }
                    ],
        }

        // tray icon
        //     QtPlatform.SystemTrayIcon {
        //         visible: true
        //         icon.name: "clock"
        //
        //         onActivated: {
        //             appwindow.show()
        //             appwindow.raise()
        //             appwindow.requestActivate()
        //         }
        //
        //         menu: QtPlatform.Menu {
        //             visible: false
        //             QtPlatform.MenuItem {
        //                 text: qsTr("Quit")
        //                 onTriggered: Qt.quit()
        //             }
        //         }
        //     }
    }
}
