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

import kclock 1.0

import "../components"

ColumnLayout {
    id: root

    property string filterText: ""
    
    signal closeRequested()

    Kirigami.SearchField {
        id: searchField
        Layout.fillWidth: true
        
        onTextChanged: {
            AddLocationSearchModel.setFilterFixedString(text)
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
            id: listView
            currentIndex: -1
            reuseItems: true
            
            function help() {
                root.closeRequested()
            }
            
            Kirigami.PlaceholderMessage {
                anchors.centerIn: parent 
                visible: parent.count == 0
                text: i18n("No locations found")
                icon.name: "globe"
            }

            model: AddLocationSearchModel
            
            delegate: ListDelegate {
                width: listView.width
                showSeparator: model.index != listView.count - 1
                
                leftPadding: Kirigami.Units.largeSpacing
                rightPadding: Kirigami.Units.largeSpacing
                topPadding: Kirigami.Units.largeSpacing
                bottomPadding: Kirigami.Units.largeSpacing
                
                onClicked: {
                    AddLocationSearchModel.addLocation(model.index);
                    ListView.view.help();
                }
                
                contentItem: RowLayout {
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.smallSpacing
                        Label {
                            text: model.city
                            font.weight: Font.Bold
                        }
                        Label {
                            text: model.country
                        }
                    }
                    
                    Label {
                        Layout.alignment: Qt.AlignRight
                        text: model.currentTime
                    }
                }
            }
        }
    }
}
