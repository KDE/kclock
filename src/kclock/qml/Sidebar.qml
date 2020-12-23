/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
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

import org.kde.kirigami 2.11 as Kirigami
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1 as QQC2

Kirigami.GlobalDrawer {
    id: drawer
    title: i18n("Clock")

    modal: false
    width: 200
    bannerVisible: false

    Kirigami.Theme.colorSet: Kirigami.Theme.Window

    header: Kirigami.AbstractApplicationHeader {
        topPadding: Kirigami.Units.smallSpacing
        bottomPadding: Kirigami.Units.largeSpacing
        leftPadding: Kirigami.Units.largeSpacing
        rightPadding: Kirigami.Units.largeSpacing
        implicitHeight: Kirigami.Units.gridUnit * 2
        
        Kirigami.Heading {
            level: 1
            text: i18n("Clock")
            Layout.fillWidth: true
            
            // HACK: spacing with other header is misaligned, so align it manually
            x: Kirigami.Units.largeSpacing
            y: Kirigami.Units.gridUnit * 0.13
        }
    }

    actions: [
        Kirigami.Action {
            text: i18n("Time")
            iconName: "clock"
            onTriggered: appwindow.switchToPageUrl(appwindow.getPage("Time"), 0)
        },
        Kirigami.Action {
            text: i18n("Timer")
            iconName: "player-time"
            onTriggered: appwindow.switchToPageUrl(appwindow.getPage("Timer"), 0)
        },
        Kirigami.Action {
            text: i18n("Stopwatch")
            iconName: "chronometer"
            onTriggered: appwindow.switchToPageUrl(appwindow.getPage("Stopwatch"), 0)
        },
        Kirigami.Action {
            text: i18n("Alarm")
            iconName: "notifications"
            onTriggered: appwindow.switchToPageUrl(appwindow.getPage("Alarm"), 0)
        },
        Kirigami.Action {
            text: i18n("Settings")
            icon.name: "settings-configure"
            onTriggered: appwindow.switchToPageUrl(appwindow.getPage("Settings"), 0)
        }
    ]
}
