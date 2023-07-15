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
    width: Kirigami.Settings.isMobile ? 360 : 550
    height: Kirigami.Settings.isMobile ? 720 : 500

    title: i18n("Clock")
//    This is okay
//    globalDrawer: Kirigami.GlobalDrawer {
//        width: 100
//        modal: false
//        contentItem: Text {
//            text: "drawer"
//        }
//    }

    // This is not
    Loader {
        source: "qrc:/qml/Sidebar.qml"
        active: true
        onLoaded: globalDrawer = item
    }
}
