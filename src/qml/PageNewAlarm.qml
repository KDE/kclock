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
import org.kde.kirigami 2.4 as Kirigami
import kirigamiclock 1.0

Kirigami.ScrollablePage {
    title: "New Alarm"

    actions {
        main: Kirigami.Action {
            iconName: "checkmark"
            text: "Done"
            onTriggered: pageStack.pop()
        }
        right: Kirigami.Action {
            iconName: "dialog-cancel"
            text: "Cancel"
            onTriggered: pageStack.pop()
        }
    }

//    function formatText(count, modelData) {
//        let data = count === 12 ? modelData + 1 : modelData;
//        return data.toString().length < 2 ? "0" + data : data;
//    }
//
//    Component {
//        id: delegateComponent
//
//        Label {
//            text: formatText(Tumbler.tumbler.count, modelData)
//            opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
//            horizontalAlignment: Text.AlignHCenter
//            verticalAlignment: Text.AlignVCenter
//        }
//    }

    ColumnLayout {
        Kirigami.FormLayout {
            id: layout
            Layout.fillWidth: true

            RowLayout {
                Kirigami.FormData.label: "Time:"
//                Tumbler {
//                    id: selectedHour
//                    model: 12
//                    delegate: delegateComponent
//                }
                SpinBox {
                    id: selectedHour
                    to: 12
                }
                Text {
                    text: ":"
                }
                SpinBox {
                    id: selectedMinute
                    to: 59
                }
                ComboBox {
                    id: selectedAmPm
                    implicitWidth: 60
                    model: ["AM", "PM"]
                }
//                Tumbler {
//                    id: selectedMinute
//                    model: 60
//                    delegate: delegateComponent
//                }
//
//                Tumbler {
//                    id: selectedAmPm
//                    model: ["AM", "PM"]
//                    delegate: delegateComponent
//                }
            }

            Kirigami.Separator {
                Kirigami.FormData.isSection: true
            }

            Flow {
                Kirigami.FormData.label: "Repeat:"
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
                Kirigami.FormData.label: "Name (optional):"
            }

        }
    }

}