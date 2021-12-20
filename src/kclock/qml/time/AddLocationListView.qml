/*
 * Copyright 2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import QtQuick.Shapes 1.12

import org.kde.kirigami 2.15 as Kirigami

import "../components"
import kclock 1.0

ListView {
    id: root
    
    property string filterText: ""
    property bool addPadding: false
    
    signal closeRequested()
    
    Component.onCompleted: root.filterText = "";
    
    currentIndex: -1
    reuseItems: true
    
    header: Control {
        width: root.width
        leftPadding: Kirigami.Units.largeSpacing * (root.addPadding ? 1 : 0)
        rightPadding: Kirigami.Units.largeSpacing * (root.addPadding ? 1 : 0)
        topPadding: Kirigami.Units.largeSpacing * (root.addPadding ? 1 : 0)
        bottomPadding: Kirigami.Units.largeSpacing 
        
        contentItem: Kirigami.SearchField {
            id: searchField
            
            onTextChanged: {
                AddLocationSearchModel.setFilterFixedString(text)
                root.filterText = text
                forceActiveFocus();
                focus = true
            }
        }
    }
    
    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent 
        visible: root.count == 0
        text: i18n("No locations found")
        icon.name: "globe"
    }

    model: AddLocationSearchModel
    
    delegate: ListDelegate {
        width: root.width
        showSeparator: model.index != root.count - 1
        
        leftPadding: Kirigami.Units.largeSpacing * (root.addPadding ? 2 : 1)
        rightPadding: Kirigami.Units.largeSpacing * (root.addPadding ? 2 : 1)
        topPadding: Kirigami.Units.largeSpacing
        bottomPadding: Kirigami.Units.largeSpacing
        
        onClicked: {
            AddLocationSearchModel.addLocation(model.index);
            ListView.view.closeRequested();
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
