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
    visible: true
    width: 650
    height: 500

    title: i18n("Clock")

    // settings object
    property Settings settings: settings
    
    pageStack.initialPage: timepage
    
    function switchToPage(page) {
        while (pageStack.depth > 0) pageStack.pop()
        pageStack.push(page)
    }
    
    globalDrawer: Kirigami.GlobalDrawer {
        title: "Clock"
        modal: !wideScreen
        bannerVisible: true
        width: 200

        actions: [
            Kirigami.Action {
                text: i18n("Time")
                iconName: "clock"
                onTriggered: switchToPage(timepage)
            },
            Kirigami.Action {
                text: i18n("Timer")
                iconName: "player-time"
                onTriggered: switchToPage(pagetimer)
            },
            Kirigami.Action {
                text: i18n("Stopwatch")
                iconName: "chronometer"
                onTriggered: switchToPage(pagestopwatch)
            },
            Kirigami.Action {
                text: i18n("Alarm")
                iconName: "notifications"
                onTriggered: switchToPage(pagealarm)
            },
            Kirigami.Action {
                text: i18n("Settings")
                iconName: "settings-configure"
                onTriggered: switchToPage(pagesettings)
            }
        ]
        
        Connections {
            target: pageStack
            onCurrentIndexChanged: { // drop all sub pages
                if (pageStack.currentIndex === 0)
                    while (pageStack.depth > 1) pageStack.pop();
            }
        }
    }
    
    // clock fonts
    FontLoader {
        id: clockFont;
        source: "/assets/RedHatText-Regular.ttf"
    }
    
    // pages
    PageTime {
        id: timepage
        objectName: "time"
    }
    PageTimer {
        id: pagetimer
        objectName: "timer"
        visible: false
    }
    PageStopwatch {
        id: pagestopwatch
        objectName: "stopwatch"
        visible: false
    }
    PageAlarm {
        id: pagealarm
        objectName: "alarm"
        visible: false
    }
    PageSettings {
        id: pagesettings
        objectName: "settings"
        visible: false
    }
    // TODO move to c++
    Kirigami.AboutPage {
        id: pageabout
        visible: false
        aboutData: {
            "displayName": "Clock",
            "productName": "kirigami/clock",
            "componentName": "kclock",
            "shortDescription": "A mobile friendly clock app built with Kirigami.",
            "homepage": "",
            "bugAddress": "",
            "version": "0.1",
            "otherText": "",
            "copyrightStatement": "© 2020 Plasma Development Team",
            "desktopFileName": "org.kde.kclock",
            "authors": [
                {
                    "name": "Devin Lin",
                    "emailAddress": "espidev@gmail.com",
                    "webAddress": "https://espi.dev",
                }
            ],
            "licenses" : [
                {
                    "name" : "GPL v2",
                    "text" : "long, boring, license text",
                    "spdx" : "GPL-2.0",
                }
            ],
        }
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
    Settings {
        id: settings
    }
}
