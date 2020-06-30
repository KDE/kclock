/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
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

Kirigami.ScrollablePage {
    title: i18n("Settings")
    
    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0
    Kirigami.ColumnView.fillWidth: false
    
    // settings list
    ColumnLayout {
        spacing: 0
        
        SwitchDelegate {
            text: i18n("Use 24 Hour Time")
            Layout.fillWidth: true
            leftPadding: Kirigami.Units.gridUnit
            rightPadding: Kirigami.Units.gridUnit
            implicitHeight: Kirigami.Units.gridUnit * 2.5
            checked: settings.use24HourTime
            onCheckedChanged: settings.use24HourTime = checked
        }
        
        Kirigami.Separator {
            Layout.fillWidth: true
        }
        
        ItemDelegate {
            Layout.fillWidth: true
            font.bold: true
            text: i18n("About")
            leftPadding: Kirigami.Units.gridUnit
            rightPadding: Kirigami.Units.gridUnit
            implicitHeight: Kirigami.Units.gridUnit * 3
            onClicked: pageStack.push(aboutPage)
        }
        
        Kirigami.Separator {
            Layout.fillWidth: true
        }
        
    }
    
    // day to start week on dialog
    Dialog {
        id: startWeekOn
        modal: true
        focus: true
        x: (pagesettings.width - width) / 2
        y: pagesettings.height / 2 - height
        width: Math.min(pagesettings.width - Kirigami.Units.gridUnit * 4, Kirigami.Units.gridUnit * 20)
        height: Kirigami.Units.gridUnit * 20
        standardButtons: Dialog.Close
        title: i18n("Start week on")
        
        contentItem: ScrollView {
            ListView {
                model: ["Saturday", "Sunday", "Monday"]
                delegate: RadioDelegate {
                    width: parent.width
                    text: i18n(modelData)
                    checked: settings.dayToStartWeekOn == modelData
                    onCheckedChanged: {
                        if (checked) 
                            settings.dayToStartWeekOn = modelData;
                    }
                }
            }
            Component.onCompleted: background.visible = true
        }
    }
}
