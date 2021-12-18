/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.19 as Kirigami

import "../components"

Kirigami.ScrollablePage {
    id: root
    
    property int yTranslate: 0
    
    title: i18n("Settings")
    icon.name: "settings-configure"
    
    Kirigami.ColumnView.fillWidth: false
    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.View
    
    // settings list
    ColumnLayout {
        transform: Translate { y: yTranslate }
        spacing: 0

        Kirigami.FormLayout {
            id: form
            Layout.fillWidth: true
            Layout.maximumWidth: root.width - root.leftPadding - root.rightPadding
            wideMode: false
            
            DialogComboBox {
                id: snoozeLengthPicker
                implicitWidth: form.width
                
                Kirigami.FormData.label: i18n("Time Format:")
                title: i18n("Select Time Format")
                text: {
                    switch (settingsModel.timeFormat) {
                        case "SystemDefault":
                            return i18n("Use System Default")
                        case "12Hour":
                            return i18n("12 Hour Time")
                        case "24Hour":
                            return i18n("24 Hour Time")
                        default:
                            return "";
                    }
                }
                
                model: ListModel {
                    // we can't use i18n with ListElement
                    Component.onCompleted: {
                        append({"name": i18n("Use System Default"), "value": "SystemDefault"});
                        append({"name": i18n("12 Hour Time"), "value": "12Hour"});
                        append({"name": i18n("24 Hour Time"), "value": "24Hour"});
                    }
                }
                
                dialogDelegate: RadioDelegate {
                    implicitWidth: Kirigami.Units.gridUnit * 16
                    topPadding: Kirigami.Units.smallSpacing * 2
                    bottomPadding: Kirigami.Units.smallSpacing * 2
                    
                    text: name
                    checked: settingsModel.timeFormat == value
                    onCheckedChanged: {
                        if (checked) {
                            settingsModel.timeFormat = value;
                        }
                    }
                }
            }
            
            Button {
                Kirigami.FormData.label: i18n("More Info:")
                text: i18n("About")
                icon.name: "help-about-symbolic"
                onClicked: applicationWindow().pageStack.layers.push(applicationWindow().getPage("About"))
            }
        }
    }
}
