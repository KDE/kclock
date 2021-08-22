/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import QtQuick.Shapes 1.12
import org.kde.kirigami 2.15 as Kirigami

Kirigami.ScrollablePage {
    id: timePage
    
    property int yTranslate: 0

    property int clockRadius: Kirigami.Units.gridUnit * 4

    title: i18n("Time")
    icon.name: "clock"

    mainAction: Kirigami.Action {
        iconName: "globe"
        text: i18n("Edit")
        onTriggered: timePage.openEditSheet()
    }
    
    function openEditSheet() {
        timeZoneSelectorModel.update();
        timeZoneSelect.active = true;
        timeZoneSelect.item.open();
    }
    
    TimeZoneSelectWrapper {
        id: timeZoneSelect
        active: false
    }
    
    header: ColumnLayout {
            Label {
                Layout.alignment: Qt.AlignHCenter
                font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 2.8)
                font.weight: Font.Light
                text: kclockFormat.currentTime
            }
            Label {
                Layout.alignment: Qt.AlignHCenter
                font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 1.2)
                text: utilModel.tzName
                color: Kirigami.Theme.textColor
            }
   }
    
    // time zones
    ListView {
        model: timeZoneShowModel
        id: zoneList
        currentIndex: -1 // no default selection
        transform: Translate { y: yTranslate }
        
        reuseItems: true
        
        add: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: Kirigami.Units.shortDuration }
        }
        remove: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: Kirigami.Units.shortDuration }
        }
        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: Kirigami.Units.longDuration; easing.type: Easing.InOutQuad}
        }
        
        // no timezones placeholder
        Kirigami.PlaceholderMessage {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: Math.round(parent.height / 2)
            visible: zoneList.count == 0
            text: i18n("No timezones configured")
            icon.name: "globe"
        }
        
        delegate: Kirigami.BasicListItem {
            leftPadding: Kirigami.Units.largeSpacing * 2
            rightPadding: Kirigami.Units.largeSpacing * 2
            topPadding: Kirigami.Units.largeSpacing
            bottomPadding: Kirigami.Units.largeSpacing
            activeBackgroundColor: "transparent"
            activeTextColor: Kirigami.Theme.textColor

            label: model.id
            subtitle: model.relativeTime
            bold: true
            
            trailing: Item {
                implicitWidth: timeText.width
                implicitHeight: timeText.height
                Kirigami.Heading {
                    id: timeText
                    level: 2
                    text: model.timeString
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }
}
