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
    }

    customFooterActions: [
        Kirigami.Action {
            icon.name: "list-add"
            text: i18n("Save As Preset")
            onTriggered: {
                TimerPresetModel.insertPreset(timerForm.name, timerForm.getDuration());
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
                TimerModel.addNew(timerForm.getDuration(), timerForm.name, timerForm.commandTimeout);
                close();
            }
        }
    ]
}
