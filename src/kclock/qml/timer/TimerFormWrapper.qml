/*
 * Copyright 2021 Devin Lin <espidev@gmail.com>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2

import org.kde.kirigami 2.19 as Kirigami

import kclock 1.0

Loader {
    id: loader
    sourceComponent: Kirigami.Settings.isMobile ? mobileTimerForm : desktopTimerForm

    function createTimer(duration, label, commandTimeout) {
        TimerModel.addNew(duration, label, commandTimeout);
    }

    property bool showPresets: false
    property bool showDelete: false

    Component {
        id: mobileTimerForm
        Kirigami.OverlayDrawer {
            height: contents.implicitHeight + Kirigami.Units.largeSpacing
            width: loader.parent.width
            edge: Qt.BottomEdge
            parent: applicationWindow().overlay

            Behavior on height {
                NumberAnimation { duration: Kirigami.Units.shortDuration }
            }
            
            ColumnLayout {
                id: contents
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 0

                Kirigami.Icon {
                    Layout.margins: Kirigami.Units.smallSpacing
                    source: "arrow-down"
                    implicitWidth: Kirigami.Units.gridUnit
                    implicitHeight: Kirigami.Units.gridUnit
                    Layout.alignment: Qt.AlignHCenter
                }

                Kirigami.Heading {
                    level: 3
                    text: i18n("<b>Create New Timer</b>")
                    Layout.alignment: Qt.AlignHCenter
                }

                TimerForm {
                    id: timerForm
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    Layout.bottomMargin: Kirigami.Units.largeSpacing
                    Layout.fillWidth: true
                    wideMode: false
                }

                RowLayout {
                    Layout.margins: Kirigami.Units.largeSpacing
                    Item { Layout.fillWidth: true }
                    Button {
                        icon.name: "list-add"
                        text: i18n("Save As Preset")
                        onClicked: {
                            TimerPresetModel.insertPreset(timerForm.name, timerForm.getDuration());
                        }
                    }
                    Button {
                        icon.name: "dialog-cancel"
                        text: i18n("Cancel")
                        onClicked: close()
                    }
                    Button {
                        icon.name: "dialog-ok"
                        text: i18n("Done")
                        onClicked: {
                            loader.createTimer(timerForm.getDuration(), timerForm.name, timerForm.commandTimeout);
                            close();
                        }
                    }
                }
            }
        }
    }

    Component {
        id: desktopTimerForm
        Kirigami.Dialog {
            title: i18n("Create timer")
            standardButtons: Dialog.NoButton
            
            topPadding: 0
            bottomPadding: Kirigami.Units.largeSpacing
            leftPadding: Kirigami.Units.gridUnit
            rightPadding: Kirigami.Units.gridUnit
            preferredWidth: Kirigami.Units.gridUnit * 20

            ColumnLayout {
                TimerForm {
                    id: timerForm
                    Layout.fillWidth: true
                }
                Flow {
                    spacing: Kirigami.Units.smallSpacing
                    visible: showPresets
                    Layout.fillWidth: true

                    Repeater {
                        model: TimerPresetModel

                        Button {
                            text: showDelete ? "Delete" : preset.presetName
                            onClicked: showDelete ? TimerPresetModel.deletePreset(index) : loader.createTimer(timerForm.getDuration(), timerForm.name) & close();
                        }
                    }
                }
            }

            customFooterActions: [
                Kirigami.Action {
                    iconName: "list-add"
                    text: i18n("Save As Preset")
                    onTriggered: {
                        TimerPresetModel.insertPreset(timerForm.name, timerForm.getDuration());
                    }
                },
                Kirigami.Action {
                    iconName: "dialog-cancel"
                    text: i18n("Cancel")
                    onTriggered: close()
                },
                Kirigami.Action {
                    iconName: "dialog-ok"
                    text: i18n("Done")
                    onTriggered: {
                        loader.createTimer(timerForm.getDuration(), timerForm.name, timerForm.commandTimeout);
                        close();
                    }
                }
            ]
        }
    }
}
