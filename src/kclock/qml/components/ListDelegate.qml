/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.12 as Kirigami

Control {
    id: root
    
    signal clicked()
    
    leftPadding: Kirigami.Units.largeSpacing
    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing
    rightPadding: Kirigami.Units.largeSpacing
    
    hoverEnabled: true
    background: Rectangle {
        color: Kirigami.Theme.textColor
        opacity: tapHandler.pressed ? 0.2 : root.hovered ? 0.1 : 0
        
        TapHandler {
            id: tapHandler
            onTapped: root.clicked()
        }
    }
}

