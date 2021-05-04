/*
 * Copyright 2021 Devin Lin <espidev@gmail.com>
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

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.15 as Kirigami
import kclock 1.0

Kirigami.FormLayout {
    id: form
    
    property alias name: label.text
    
    function setDuration(duration) {
        hours = duration / 60 / 60;
        minutes = duration % (60 * 60) / 60;
        seconds = duration % 60;
    }
    function getDuration() {
        return spinBoxHours.value * 60 * 60 + spinBoxMinutes.value * 60 + spinBoxSeconds.value;
    }

    ColumnLayout {
        id: presets
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: Kirigami.Units.smallSpacing
        RowLayout {
            spacing: Kirigami.Units.smallSpacing
            Button {
                topPadding: Kirigami.Units.largeSpacing
                bottomPadding: Kirigami.Units.largeSpacing
                implicitWidth: Kirigami.Units.gridUnit * 4
                text: i18n("1 m")
                onClicked: {
                    spinBoxHours.value = 0;
                    spinBoxMinutes.value = 1;
                    spinBoxSeconds.value = 0;
                    focus = false;
                }
            }
            Button {
                topPadding: Kirigami.Units.largeSpacing
                bottomPadding: Kirigami.Units.largeSpacing
                implicitWidth: Kirigami.Units.gridUnit * 4
                text: i18n("5 m")
                onClicked: {
                    spinBoxHours.value = 0;
                    spinBoxMinutes.value = 5;
                    spinBoxSeconds.value = 0;
                    focus = false;
                }
            }
            Button {
                topPadding: Kirigami.Units.largeSpacing
                bottomPadding: Kirigami.Units.largeSpacing
                implicitWidth: Kirigami.Units.gridUnit * 4
                text: i18n("10 m")
                onClicked: {
                    spinBoxHours.value = 0;
                    spinBoxMinutes.value = 10;
                    spinBoxSeconds.value = 0;
                    focus = false;
                }
            }
        }
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: Kirigami.Units.largeSpacing
            spacing: Kirigami.Units.smallSpacing
            Button {
                topPadding: Kirigami.Units.largeSpacing
                bottomPadding: Kirigami.Units.largeSpacing
                implicitWidth: Kirigami.Units.gridUnit * 4
                text: i18n("15 m")
                onClicked: {
                    spinBoxHours.value = 0;
                    spinBoxMinutes.value = 15;
                    spinBoxSeconds.value = 0;
                    focus = false;
                }
            }
            Button {
                topPadding: Kirigami.Units.largeSpacing
                bottomPadding: Kirigami.Units.largeSpacing
                implicitWidth: Kirigami.Units.gridUnit * 4
                text: i18n("30 m")
                onClicked: {
                    spinBoxHours.value = 0;
                    spinBoxMinutes.value = 30;
                    spinBoxSeconds.value = 0;
                    focus = false;
                }
            }
            Button {
                topPadding: Kirigami.Units.largeSpacing
                bottomPadding: Kirigami.Units.largeSpacing
                implicitWidth: Kirigami.Units.gridUnit * 4
                text: i18n("1 h")
                onClicked: {
                    spinBoxHours.value = 1;
                    spinBoxMinutes.value = 0;
                    spinBoxSeconds.value = 0;
                    focus = false;
                }
            }
        }
    }
    
    RowLayout {
        Kirigami.FormData.label: i18n("<b>Duration:</b>")
        SpinBox {
            Layout.alignment: Qt.AlignVCenter
            id: spinBoxHours
            value: 0 // default
        }
        Label {
            Layout.alignment: Qt.AlignVCenter
            text: i18n("hours")
        }
    }
    RowLayout {
        SpinBox {
            Layout.alignment: Qt.AlignVCenter
            id: spinBoxMinutes
            value: 5 // default
        }
        Label {
            Layout.alignment: Qt.AlignVCenter
            text: i18n("minutes")
        }
    }
    RowLayout {
        SpinBox {
            Layout.alignment: Qt.AlignVCenter
            id: spinBoxSeconds
            to: 60
            value: 0 // default
        }
        Label {
            Layout.alignment: Qt.AlignVCenter
            text: i18n("seconds")
        }
    }

    TextField {
        id: label
        Kirigami.FormData.label: i18n("<b>Label (optional):</b>")
        text: i18n("New Timer") // default
        onTextChanged: timer.label = text
    }
} 
