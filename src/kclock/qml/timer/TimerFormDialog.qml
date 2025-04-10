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

import org.kde.kclock

Kirigami.Dialog {
    id: root

    property Timer timer: null

    function saveTimer(duration, label, looping, commandTimeout) {
        if (timer) {
            timer.length = duration;
            timer.label = label;
            timer.commandTimeout = commandTimeout;
            timer.looping = looping;
            timer.reset();
        } else {
            TimerModel.addNew(duration, label, looping, commandTimeout);
        }
    }

    property bool showPresets: false
    property bool showDelete: false

    title: timer ? i18nc("@title:window Edit timer", "Edit %1", timer.label) : i18nc("@title:window", "Create Timer")
    standardButtons: Dialog.NoButton

    topPadding: 0
    bottomPadding: Kirigami.Units.largeSpacing
    leftPadding: Kirigami.Units.gridUnit
    rightPadding: Kirigami.Units.gridUnit
    preferredWidth: Kirigami.Units.gridUnit * 20

    onAboutToShow: {
        if (root.timer) {
            timerForm.setDuration(root.timer.length);
            timerForm.name = timer.label;
            timerForm.looping = timer.looping;
            timerForm.commandTimeout = timer.commandTimeout;
        } else {
            timerForm.setDuration(5 * 60); // 5 minutes default.
            timerForm.name = i18n("Timer");
            timerForm.looping = false;
            timerForm.commandTimeout = "";
        }
    }

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
                onClicked: root.showPresets = !root.showPresets
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
            visible: root.showPresets
            Layout.fillWidth: true

            Repeater {
                id: repeater
                model: TimerPresetModel

                Button {
                    text: root.showDelete ? "Delete" : preset.presetName
                    onClicked: root.showDelete ? TimerPresetModel.deletePreset(index) : root.saveTimer(preset.presetDuration, preset.presetName, false, "") & close();
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
            onTriggered: root.close()
        },
        Kirigami.Action {
            icon.name: root.timer ? "document-save" : "dialog-ok"
            text: root.timer ? i18nc("@action:button", "Save") : i18nc("@action:button", "Done")
            onTriggered: {
                root.saveTimer(timerForm.getDuration(), timerForm.name, timerForm.looping, timerForm.commandTimeout);
                root.close();
            }
        }
    ]
}
