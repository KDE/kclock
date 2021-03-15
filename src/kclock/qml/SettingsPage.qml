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
    icon.name: "settings-configure"
    topPadding: 0
    bottomPadding: 0
    leftPadding: 0
    rightPadding: 0
    Kirigami.ColumnView.fillWidth: false

    // settings list
    ColumnLayout {
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
        }

    }

    // alarm volume dialog
    Dialog {
        id: alarmVolumeDialog
        modal: true
        focus: true
        anchors.centerIn: Overlay.overlay
        width: Math.min(appwindow.width - Kirigami.Units.gridUnit * 4, Kirigami.Units.gridUnit * 20)
        height: Kirigami.Units.gridUnit * 8
        standardButtons: Dialog.Close
        title: i18n("Change Alarm Volume")
        contentItem: RowLayout {
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
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
        onClosed: {alarmPlayer.stop();
                    settingsModel.volume = volumeControl.value;
        }
    }

    // Silence alarm after dialog
    Dialog {
        id: silenceAlarmAfter
        modal: true
        focus: true
        anchors.centerIn: Overlay.overlay
        width: Math.min(appwindow.width - Kirigami.Units.gridUnit * 4, Kirigami.Units.gridUnit * 20)
        height: Kirigami.Units.gridUnit * 20
        standardButtons: Dialog.Close
        title: i18n("Silence Alarm After")

        contentItem: ScrollView {
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ListView {
                model: ListModel {
                    ListElement {
                        name: i18n("30 seconds")
                        value: 30
                    }
                    ListElement {
                        name: i18n("1 minute")
                        value: 60
                    }
                    ListElement {
                        name: i18n("5 minutes")
                        value: 300
                    }
                    ListElement {
                        name: i18n("10 minutes")
                        value: 600
                    }
                    ListElement {
                        name: i18n("15 minutes")
                        value: 900
                    }
                    ListElement {
                        name: i18n("Never")
                        value: -1
                    }
                }
                delegate: RadioDelegate {
                    width: parent.width
                    text: i18n(name)
                    checked: settingsModel && settingsModel.alarmSilenceAfter == value
                    onCheckedChanged: {
                        if (checked) {
                            settingsModel.alarmSilenceAfter = value;
                            settingsModel.alarmSilenceAfterDisplay = name;
                        }
                    }
                }
            }
            Component.onCompleted: background.visible = true
        }
    }

    // Alarm snooze length dialog
    Dialog {
        id: alarmSnoozeLength
        modal: true
        focus: true
        anchors.centerIn: Overlay.overlay
        width: Math.min(appwindow.width - Kirigami.Units.gridUnit * 4, Kirigami.Units.gridUnit * 20)
        height: Kirigami.Units.gridUnit * 20
        standardButtons: Dialog.Close
        title: i18n("Alarm Snooze Length")

        contentItem: ScrollView {
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ListView {
                model: ListModel {
                    ListElement {
                        name: i18n("1 minute")
                        value: 1
                    }
                    ListElement {
                        name: i18n("2 minutes")
                        value: 2
                    }
                    ListElement {
                        name: i18n("3 minutes")
                        value: 3
                    }
                    ListElement {
                        name: i18n("4 minutes")
                        value: 4
                    }
                    ListElement {
                        name: i18n("5 minutes")
                        value: 5
                    }
                    ListElement {
                        name: i18n("10 minutes")
                        value: 10
                    }
                    ListElement {
                        name: i18n("30 minutes")
                        value: 30
                    }
                    ListElement {
                        name: i18n("1 hour")
                        value: 60
                    }
                }
                delegate: RadioDelegate {
                    width: parent.width
                    text: i18n(name)
                    checked: settingsModel && settingsModel.alarmSnoozeLength == value
                    onCheckedChanged: {
                        if (checked) {
                            settingsModel.alarmSnoozeLength = value;
                            settingsModel.alarmSnoozeLengthDisplay = name;
                        }
                    }
                }
            }
            Component.onCompleted: background.visible = true
        }
    }
}
