/*
 * Copyright 2021 Devin Lin <espidev@gmail.com>
 * Copyright 2021 Boris Petrov <boris.v.petrov@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import kclock

Kirigami.Dialog {
    id: root

    function createTimer(duration, label, commandTimeout) {
        TimerModel.addNew(duration, label, commandTimeout);
    }

    property bool showPresets: false
    property bool showDelete: false

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
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: Kirigami.Units.smallSpacing
            visible: repeater.count > 0

            Button {
                id: presetButton
                text: root.showPresets ? i18n("Hide Presets") : i18n("Show Presets")
                onClicked: showPresets = !showPresets
            }
            ToolButton {
                icon.name: "delete"
                text: i18n("Toggle Delete")
                onClicked: root.showDelete = !root.showDelete
                visible: root.showPresets
                checkable: true
                checked: false
            }
        }
        Flow {
            spacing: Kirigami.Units.smallSpacing
            visible: showPresets
            Layout.fillWidth: true

            Repeater {
                id: repeater
                model: TimerPresetModel

                Button {
                    text: showDelete ? "Delete" : preset.presetName
                    onClicked: showDelete ? TimerPresetModel.deletePreset(index) : root.createTimer(timerForm.getDuration(), timerForm.name) & close();
                }
            }
        }
    }

    customFooterActions: [
        Kirigami.Action {
            icon.name: "list-add"
            text: i18n("Save As Preset")
            onTriggered: {
                TimerPresetModel.insertPreset(timerForm.name, timerForm.getDuration());
                root.showPresets = true;
            }
        },
        Kirigami.Action {
            icon.name: "dialog-cancel"
            text: i18n("Cancel")
            onTriggered: close()
        },
        Kirigami.Action {
            icon.name: "dialog-ok"
            text: i18n("Done")
            onTriggered: {
                root.createTimer(timerForm.getDuration(), timerForm.name, timerForm.commandTimeout);
                close();
            }
        }
    ]
}
