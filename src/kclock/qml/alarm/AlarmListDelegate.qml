/*
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2023 Nate Graham <nate@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.kclock

Delegates.RoundedItemDelegate {
    id: root

    property Alarm alarm

    // avoid null errors when deleting delegate
    readonly property bool enabled: alarm ? alarm.enabled : false
    readonly property string name: alarm ? alarm.name : ""
    readonly property string formattedTime: alarm ? alarm.formattedTime : ""
    readonly property int daysOfWeek: alarm ? alarm.daysOfWeek : 0
    readonly property int snoozedLength: alarm ? alarm.snoozedLength : 0

    property bool editMode: false

    topPadding: Kirigami.Settings.isMobile ? Kirigami.Units.gridUnit : Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Settings.isMobile ? Kirigami.Units.gridUnit : Kirigami.Units.largeSpacing
    leftPadding: Kirigami.Units.gridUnit
    rightPadding: Kirigami.Units.gridUnit

    signal editClicked()
    signal deleteClicked()

    onClicked: editClicked()

    // alarm ringing popup
    Loader {
        id: popupLoader
        active: false
        sourceComponent: AlarmRingingPopup {
            alarm: root.alarm
            onVisibleChanged: {
                if (!visible) {
                    popupLoader.active = false;
                }
            }
        }

        Component.onCompleted: determineState()
        function determineState() {
            if (root.alarm.ringing) {
                popupLoader.active = true;
                popupLoader.item.open();
            } else if (popupLoader.item) {
                popupLoader.item.close();
            }
        }

        Connections {
            target: root.alarm
            ignoreUnknownSignals: true

            function onRingingChanged() {
                popupLoader.determineState();
            }
        }
    }

    contentItem: RowLayout {
        spacing: Kirigami.Units.smallSpacing

        ColumnLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing
            opacity: root.enabled ? 1.0 : 0.7

            Label {
                id: label
                Layout.fillWidth: true
                text: root.formattedTime
                font.bold: true
            }

            Label {
                id: subtitle
                Layout.fillWidth: true
                text: {
                    let subtitleString = "";
                    if (root.name.length > 0) {
                        subtitleString = subtitleString + root.name + " - ";
                    }

                    subtitleString += UtilModel.repeatFormat(root.daysOfWeek);

                    if (root.snoozedLength > 0) {
                        subtitleString += "\n"
                        subtitleString += i18np("Snoozed %1 minute", "Snoozed %1 minutes", root.snoozedLength);
                    }
                    return subtitleString;
                }
                textFormat: Text.PlainText
            }
        }

        Switch {
            id: toggleSwitch

            // can't use Connections since it conflicts with enabled property
            property bool alarmEnabled: root.enabled
            onAlarmEnabledChanged: checked = root.enabled

            checked: root.enabled
            onCheckedChanged: root.alarm.enabled = checked;
        }

        ToolButton {
            icon.name: "delete"
            text: i18nc("@action:button", "Delete")
            onClicked: root.deleteClicked()
            visible: root.editMode
            display: AbstractButton.IconOnly

            ToolTip.delay: Kirigami.Units.toolTipDelay
            ToolTip.visible: Kirigami.Settings.tabletMode ? pressed : hovered
            ToolTip.text: text
        }
    }
}
