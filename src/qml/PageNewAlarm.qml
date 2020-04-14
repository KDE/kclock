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
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.12 as Kirigami
import kirigamiclock 1.0

Kirigami.ScrollablePage {
    title: newAlarm ? i18n("New Alarm") : i18n("Edit Alarm")

    actions {
        main: Kirigami.Action {
            iconName: "checkmark"
            text: i18n("Done")
            onTriggered: {
                if (newAlarm) {
                    alarmModel.addAlarm(name.text, selectedMinute.value, selectedHour.value, "1,2");
                } else {
                    selectedAlarm.name = name.text;
                    selectedAlarm.minutes = selectedMinute.value;
                    selectedAlarm.hours = selectedHour.value;
                }
                pageStack.pop()
            }
        }
        right: Kirigami.Action {
            iconName: "dialog-cancel"
            text: i18n("Cancel")
            onTriggered: pageStack.pop()
        }
    }

    ColumnLayout {
        Kirigami.FormLayout {
            id: layout
            Layout.fillWidth: true

            RowLayout {
                Kirigami.FormData.label: i18n("Time") + ":"
                SpinBox {
                    id: selectedHour
                    to: 12
                    value: newAlarm ? 0 : selectedAlarm.hours
                    textFromValue: (value, locale) => ("0" + value).slice(-2)
                }
                Text {
                    text: ":"
                }
                SpinBox {
                    id: selectedMinute
                    to: 59
                    value: newAlarm ? 0 : selectedAlarm.minutes
                    textFromValue: (value, locale) => ("0" + value).slice(-2)
                }
                ComboBox {
                    id: selectedAmPm
                    implicitWidth: 60
                    model: ["AM", "PM"]
                }
            }

            Kirigami.Separator {
                Kirigami.FormData.isSection: true
            }

            Flow {
                Kirigami.FormData.label: i18n("Repeat") + ":"
                Button {
                    implicitWidth: 40
                    text: "S"
                }
                Button {
                    implicitWidth: 40
                    text: "M"
                }
                Button {
                    implicitWidth: 40
                    text: "T"
                }
                Button {
                    implicitWidth: 40
                    text: "W"
                }
                Button {
                    implicitWidth: 40
                    text: "T"
                }
                Button {
                    implicitWidth: 40
                    text: "F"
                }
                Button {
                    implicitWidth: 40
                    text: "S"
                }
            }

            Kirigami.Separator {
                Kirigami.FormData.isSection: true
            }

            TextField {
                id: name
                Kirigami.FormData.label: i18n("Name") + " (" + i18n("optional") + "):"
                placeholderText: i18n("Wake Up")
                text: newAlarm ? "" : selectedAlarm.name
            }

        }
    }

}