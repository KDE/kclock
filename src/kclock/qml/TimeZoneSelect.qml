/*
 * Copyright 2020 Nicolas Fella <nicolas.fella@gmx.de>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.12 as Kirigami

ColumnLayout {
    id: root
    property string filterText: ""
    Kirigami.SearchField {
        id: searchField
        Layout.fillWidth: true
        
        onTextChanged: {
            timeZoneFilterModel.setFilterFixedString(text)
            root.filterText = text
            forceActiveFocus();
            focus = true
        }
    }
    ScrollView {
        Layout.minimumHeight: Kirigami.Units.gridUnit * 14
        Layout.maximumHeight: Kirigami.Units.gridUnit * 20
        Layout.preferredHeight: Kirigami.Units.gridUnit * (Kirigami.Settings.isMobile ? 20 : 14) // mobile drawer should be taller
        Layout.fillWidth: true
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ListView {
            currentIndex: -1
            reuseItems: true
            
            Kirigami.PlaceholderMessage {
                anchors.centerIn: parent 
                visible: parent.count == 0
                text: i18n("Search for a city")
                icon.name: "globe"
            }

            model: root.filterText == "" ? [] : timeZoneFilterModel // only display cities if there is a query (for performance)
            delegate: Kirigami.AbstractListItem {
                activeBackgroundColor: "transparent"
                CheckBox {
                    checked: model.shown
                    text: i18n("%1 %2", model.id, model.shortName)
                    onClicked: model.shown = this.checked
                }
            }
        }
    }
}
