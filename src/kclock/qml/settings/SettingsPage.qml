/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import "../components"
import kclock

FormCard.FormCardPage {
    id: root

    property real yTranslate: 0

    title: i18n("Settings")
    icon.name: "settings-configure"

    Kirigami.ColumnView.fillWidth: false

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormComboBoxDelegate {
            text: i18n("Time Format:")
            model: ListModel {
                // we can't use i18n with ListElement
                Component.onCompleted: {
                    append({"name": i18n("Use System Default"), "value": "SystemDefault"});
                    append({"name": i18n("12 Hour Time"), "value": "12Hour"});
                    append({"name": i18n("24 Hour Time"), "value": "24Hour"});
                }
            }

            textRole: "name"
            valueRole: "value"

            onCurrentValueChanged: SettingsModel.timeFormat = currentValue

            Component.onCompleted: {
                const index = indexOfValue(SettingsModel.timeFormat);
                currentIndex = index == -1 ? 0 : index;
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            text: i18n("About Clock")
            icon.name: "org.kde.kclock"
            onClicked: applicationWindow().pageStack.push(applicationWindow().getPage("About"))
        }
    }
}
