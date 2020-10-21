/*
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *           2020 Devin Lin <espidev@gmail.com>
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
import org.kde.kirigami 2.11 as Kirigami
import kclock 1.0

Kirigami.Page {
    
    property Timer timer
    property int timerIndex
    
    id: timerpage
    title: timer.label !== "" ? timer.label : i18n("New timer")
    
    property bool justCreated: timer == null ? false : timer.justCreated
    property bool showFullscreen: false
    
    property int elapsed: timer == null ? 0 : timer.elapsed
    property int duration: timer == null ? 0 : timer.length
    property bool running: timer == null ? 0 : timer.running

    // topbar action
    actions {
        main: Kirigami.Action {
            text: running ? i18n("Pause") : i18n("Start")
            iconName: running ? "chronometer-pause" : "chronometer-start"
            onTriggered: timer.toggleRunning()
            visible: !justCreated
        }

        contextualActions: [
            Kirigami.Action {
                visible: !justCreated
                icon.name: "chronometer-reset"
                text: i18n("Reset")
                onTriggered: timer.reset();
            },
            Kirigami.Action {
                visible: !justCreated
                icon.name: "delete"
                text: i18n("Delete")
                onTriggered: timerModel.remove(timerIndex);
            }
        ]
    }

    // create new timer form

    Kirigami.FormLayout {
        id: form
        anchors.fill: parent
        visible: justCreated

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: "Duration"
        }
        Kirigami.Separator {}

        RowLayout {
            SpinBox {
                Layout.alignment: Qt.AlignVCenter
                id: spinBoxHours
                onValueChanged: form.setDuration()
                value: duration / 60 / 60
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
                onValueChanged: form.setDuration()
                value: duration % (60*60) / 60
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
                onValueChanged: form.setDuration()
                value: duration % 60
            }
            Label {
                Layout.alignment: Qt.AlignVCenter
                text: i18n("seconds")
            }
        }

        function setDuration() {
            timer.length = spinBoxHours.value * 60 * 60 + spinBoxMinutes.value * 60 + spinBoxSeconds.value
        }

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: "Label (optional)"
        }

        Kirigami.Separator {}
        TextField {
            text: timer.label
            onTextChanged: timer.label = text
        }

        Kirigami.Separator {}
        ToolButton {
            flat: false
            icon.name: "list-add"
            text: "Create"
            onClicked: justCreated = false;
        }
    }

    TimerComponent {
        visible: !justCreated
        timerDuration: duration
        timerElapsed: elapsed
        timerRunning: running
    }
}
