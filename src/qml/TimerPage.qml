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
    title: timer.label
    
    property bool justCreated: timer == null ? false : timer.justCreated
    property bool showFullscreen: false
    
    property int elapsed: timer == null ? 0 : timer.elapsed
    property int duration: timer == null ? 0 : timer.length / 1000
    property bool running: timer == null ? 0 : timer.running

    // topbar action
    mainAction: Kirigami.Action {
        text: running ? "Pause" : "Start"
        iconName: running ? "chronometer-pause" : "chronometer-start"
        onTriggered: timer.toggleRunning()
        visible: !justCreated
    }

    // create new timer form
    ColumnLayout {
        spacing: Kirigami.Units.largeSpacing
        visible: justCreated
        anchors.left: parent.left
        anchors.right: parent.right
        
        Kirigami.FormLayout {
            id: form
            Layout.fillWidth: true
            
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
                timer.length = 1000 * (spinBoxHours.value * 60 * 60 + spinBoxMinutes.value * 60 + spinBoxSeconds.value)
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
    }

    
    
    // ~ timer display ~
    
    Kirigami.ActionToolBar {
        visible: !justCreated
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        alignment: Qt.AlignHCenter
        actions: [
            Action {
                icon.name: "chronometer-reset"
                text: "Reset"
                onTriggered: timer.reset();
            },
//             Action {
//                 icon.name: "view-fullscreen"
//                 text: "Fullscreen"
//                 onTriggered: showFullscreen = true;
//                 
//             },
            Action {
                icon.name: "delete"
                text: "Delete"
                onTriggered: timerModel.remove(timerIndex); 
            }
        ]
    }

    TimerComponent {
        visible: !justCreated
        timerDuration: duration
        timerElapsed: elapsed
    }
}
