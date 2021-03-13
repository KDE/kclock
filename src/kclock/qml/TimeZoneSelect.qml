/*
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *           2020 Devin Lin <espidev@gmail.com>
 *           2020 Nicolas Fella <nicolas.fella@gmx.de>
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
        Layout.maximumHeight: Kirigami.Units.gridUnit * 14
        Layout.fillWidth: true
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
