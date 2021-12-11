/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.19 as Kirigami

Kirigami.ScrollablePage {
    id: settingsPage
    
    property int yTranslate: 0
    
    title: i18n("Settings")
    icon.name: "settings-configure"
    padding: 0
    Kirigami.ColumnView.fillWidth: false
    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.View
    
    // settings list
    ColumnLayout {
        transform: Translate { y: yTranslate }
        spacing: 0

        ItemDelegate {
            Layout.fillWidth: true
            implicitHeight: Kirigami.Units.gridUnit * 3

            onClicked: alarmVolumeDialog.open()

            ColumnLayout {
                spacing: -5
                anchors.leftMargin: Kirigami.Units.gridUnit
                anchors.rightMargin: Kirigami.Units.gridUnit
                anchors.fill: parent

                Label {
                    text: i18n("Alarm Volume")
                    font.weight: Font.Bold
                }
                Label {
                    text: String(settingsModel.volume)
                }
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
            Layout.leftMargin: Kirigami.Units.largeSpacing
            Layout.rightMargin: Kirigami.Units.largeSpacing
            opacity: 0.8
        }

        ItemDelegate {
            Layout.fillWidth: true
            implicitHeight: Kirigami.Units.gridUnit * 3
            
            onClicked: applicationWindow().pageStack.layers.push(applicationWindow().getPage("About"))
            
            Label {
                anchors.left: parent.left
                anchors.leftMargin: Kirigami.Units.gridUnit
                anchors.verticalCenter: parent.verticalCenter
                font.weight: Font.Bold
                text: i18n("About")
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
            Layout.leftMargin: Kirigami.Units.largeSpacing
            Layout.rightMargin: Kirigami.Units.largeSpacing
            opacity: 0.8
        }
        
        // alarm volume dialog
        Kirigami.Dialog {
            id: alarmVolumeDialog
            standardButtons: Dialog.Close
            padding: Kirigami.Units.largeSpacing * 2
            
            onClosed: settingsModel.volume = volumeControl.value;
            
            title: i18n("Change Alarm Volume")
            contentItem: RowLayout {
                Label {
                    text: i18n("Volume: ")
                }
                Slider {
                    id: volumeControl
                    Layout.fillWidth: true
                    from: 0
                    to: 100
                    value: settingsModel.volume
                    onPressedChanged: {
                        if (!pressed) {
                            settingsModel.volume
                        }
                    }
                }
            }
        }
    }
}
