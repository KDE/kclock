/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.11 as Kirigami

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

            onClicked: silenceAlarmAfter.open()

            ColumnLayout {
                spacing: -5
                anchors.leftMargin: Kirigami.Units.gridUnit
                anchors.rightMargin: Kirigami.Units.gridUnit
                anchors.fill: parent

                Label {
                    text: i18n("Silence Alarm After")
                    font.weight: Font.Bold
                }
                Label {
                    text: settingsModel.alarmSilenceAfterDisplay
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

            onClicked: alarmSnoozeLength.open()

            ColumnLayout {
                spacing: -5
                anchors.leftMargin: Kirigami.Units.gridUnit
                anchors.rightMargin: Kirigami.Units.gridUnit
                anchors.fill: parent

                Label {
                    text: i18n("Alarm Snooze Length")
                    font.weight: Font.Bold
                }
                Label {
                    text: settingsModel.alarmSnoozeLengthDisplay
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
            
            onClicked: appwindow.switchToPage(appwindow.getPage("About"), 1)
            
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
        PopupDialog {
            id: alarmVolumeDialog
            standardButtons: Dialog.Close
            padding: Kirigami.Units.largeSpacing * 2
            
            onClosed: {
                alarmPlayer.stop();
                settingsModel.volume = volumeControl.value;
            }
            
            title: i18n("Change Alarm Volume")
            contentItem: RowLayout {
                id: row
                transform: Translate { y: alarmVolumeDialog.translateY }
                Label {
                    text: i18n("Volume: ")
                }
                Slider {
                    id: volumeControl
                    Layout.fillWidth: true
                    from: 0
                    to: 100
                    value: alarmPlayer.volume
                    onPressedChanged: {
                        if (!pressed) {
                            alarmPlayer.play();
                        }
                    }
                }
            }
        }

        // Silence alarm after dialog
        PopupDialog {
            id: silenceAlarmAfter
            standardButtons: Dialog.Close
            title: i18n("Silence Alarm After")

            ColumnLayout {
                Kirigami.Theme.inherit: false
                Kirigami.Theme.colorSet: Kirigami.Theme.View
                spacing: 0
                
                Repeater {
                    model: ListModel {
                        // we can't use i18n with ListElement
                        Component.onCompleted: {
                            append({"name": i18n("30 seconds"), "value": 30});
                            append({"name": i18n("1 minute"), "value": 60});
                            append({"name": i18n("5 minutes"), "value": 300});
                            append({"name": i18n("10 minutes"), "value": 600});
                            append({"name": i18n("15 minutes"), "value": 900});
                            append({"name": i18n("Never"), "value": -1});
                        }
                    }
                    delegate: RadioDelegate {
                        topPadding: Kirigami.Units.smallSpacing * 2
                        bottomPadding: Kirigami.Units.smallSpacing * 2
                        implicitWidth: Kirigami.Units.gridUnit * 16
                        
                        text: name
                        checked: settingsModel && settingsModel.alarmSilenceAfter == value
                        onCheckedChanged: {
                            if (checked) {
                                settingsModel.alarmSilenceAfter = value;
                                settingsModel.alarmSilenceAfterDisplay = name;
                            }
                        }
                    }
                }
            }
        }

        // Alarm snooze length dialog
        PopupDialog {
            id: alarmSnoozeLength
            standardButtons: Dialog.Close
            title: i18n("Alarm Snooze Length")

            ColumnLayout {
                Kirigami.Theme.inherit: false
                Kirigami.Theme.colorSet: Kirigami.Theme.View
                spacing: 0
                
                Repeater {
                    model: ListModel {
                        // we can't use i18n with ListElement
                        Component.onCompleted: {
                            append({"name": i18n("1 minute"), "value": 1});
                            append({"name": i18n("2 minutes"), "value": 2});
                            append({"name": i18n("3 minutes"), "value": 3});
                            append({"name": i18n("4 minutes"), "value": 4});
                            append({"name": i18n("5 minutes"), "value": 5});
                            append({"name": i18n("10 minutes"), "value": 10});
                            append({"name": i18n("30 minutes"), "value": 30});
                            append({"name": i18n("1 hour"), "value": 60});
                        }
                    }
                    delegate: RadioDelegate {
                        topPadding: Kirigami.Units.smallSpacing * 2
                        bottomPadding: Kirigami.Units.smallSpacing * 2
                        implicitWidth: Kirigami.Units.gridUnit * 16
                        
                        text: name
                        checked: settingsModel && settingsModel.alarmSnoozeLength == value
                        onCheckedChanged: {
                            if (checked) {
                                settingsModel.alarmSnoozeLength = value;
                                settingsModel.alarmSnoozeLengthDisplay = name;
                            }
                        }
                    }
                }
            }
        }
    }
}
