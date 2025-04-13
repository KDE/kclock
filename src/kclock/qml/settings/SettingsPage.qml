/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.kclock

FormCard.FormCardPage {
    id: root

    property real yTranslate: 0

    objectName: "Settings"
    title: i18n("Settings")
    icon.name: "settings-configure"
    background: null

    Kirigami.ColumnView.fillWidth: false

    contentItem.transform: Translate { y: root.yTranslate }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.gridUnit

        FormCard.FormComboBoxDelegate {
            id: timeFormatDelegate
            text: i18n("Time Format")
            model: [
                {name: i18n("Use System Default"), value: "SystemDefault"},
                {name: i18n("12 Hour Time"), value: "12Hour"},
                {name: i18n("24 Hour Time"), value: "24Hour"},
            ]
            currentIndex: model.findIndex(item => item.value === SettingsModel.timeFormat)

            textRole: "name"
            valueRole: "value"

            onActivated: SettingsModel.timeFormat = currentValue
        }

        FormCard.FormDelegateSeparator { above: timeFormatDelegate; below: timerNotificationDelegate }

        FormCard.FormComboBoxDelegate {
            id: timerNotificationDelegate
            text: i18n("Notifications for running timers")
            model: [{
                name: i18n("Always"),
                value: Settings.Always
            }, {
                name: i18nc("When the app is not running", "When %1 is not running", Application.displayName),
                value: Settings.WhenKClockNotRunning
            }, {
                name: i18n("Never"),
                value: Settings.Never
            }]
            description: currentValue !== Settings.Never ? i18n("Lets you pause and resume timers from the notification area.") : ""
            currentIndex: model.findIndex(item => item.value === SettingsModel.timerNotification)

            textRole: "name"
            valueRole: "value"

            onActivated: SettingsModel.timerNotification = currentValue
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.gridUnit

        FormCard.FormButtonDelegate {
            id: aboutClock
            text: i18n("About Clock")
            icon.name: "help-about-symbolic"
            onClicked: applicationWindow().pageStack.push(applicationWindow().getPage("About"))
        }

        FormCard.FormDelegateSeparator { above: aboutClock; below: aboutKde }

        FormCard.FormButtonDelegate {
            id: aboutKde
            text: i18n("About KDE")
            icon.name: "kde-symbolic"
            onClicked: applicationWindow().pageStack.push(kdePage)

            Component {
                id: kdePage
                FormCard.AboutKDEPage {}
            }
        }
    }
}
