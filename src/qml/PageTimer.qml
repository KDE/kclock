/*
 * Copyright 2019  Nick Reitemeyer <nick.reitemeyer@web.de>
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
import org.kde.kirigami 2.4 as Kirigami

Kirigami.Page {
    
    title: "Timer"
    id: timerpage
    property bool running: false
    property int timerDuration: 60
    property int elapsedTime: 0
    
    Timer {
        interval: 16
        running: timerpage.running
        repeat: true
        onTriggered: {
            elapsedTime += interval
        }
    }

    // topbar action
    mainAction: Kirigami.Action {
        text: running ? "Pause" : "Start"
        iconName: running ? "chronometer-pause" : "chronometer-start"
        onTriggered: {
            running = !running
        }
    }

    function getTimeLeft() {
        return timerDuration*1000 - elapsedTime;
    }
    function getHours() {
        return ("0" + parseInt(getTimeLeft() / 1000 / 60 / 24).toFixed(0)).slice(-2);
    }
    function getMinutes() {
        return ("0" + parseInt(getTimeLeft() / 1000 / 60 - 24*getHours())).slice(-2);
    }
    function getSeconds() {
        return ("0" + parseInt(getTimeLeft() / 1000 - 60*getMinutes())).slice(-2);
    }

    // clock display
    RowLayout {
        id: timeLabels
        anchors.horizontalCenter: parent.horizontalCenter

        Label {
            id: hoursText
            text: getHours()
            color: Kirigami.Theme.highlightColor
            font.pointSize: 40
            font.kerning: false
        }
        Text {
            text: ":"
            color: Kirigami.Theme.textColor
            font.pointSize: 40
        }
        Label {
            id: minutesText
            text: getMinutes()
            color: Kirigami.Theme.highlightColor
            font.pointSize: 40
            font.kerning: false
        }
        Text {
            text: ":"
            color: Kirigami.Theme.textColor
            font.pointSize: 40
        }
        Label {
            text: getSeconds()
            color: Kirigami.Theme.highlightColor
            font.pointSize: 40
            font.kerning: false
        }
    }

    // start/pause and lap button
    RowLayout {
        id: buttons
        anchors.topMargin: 20;
        anchors.top: timeLabels.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        Layout.alignment: Qt.AlignHCenter

        ToolButton {
            icon.name: "chronometer"
            text: "Edit"
            onClicked: {
                timerEditSheet.open()
            }
            Layout.alignment: Qt.AlignHCenter
        }
        ToolButton {
            text: "Reset"
            icon.name: "chronometer-reset"
            onClicked: {
                elapsedTime = 0
                running = false
            }
            Layout.alignment: Qt.AlignHCenter
        }
    }

    Dialog {
        id: timerEditSheet
        modal: true
        focus: true
        width: Math.min(appwindow.width - Kirigami.Units.gridUnit * 4, Kirigami.Units.gridUnit * 20)
        height: Kirigami.Units.gridUnit * 20
        standardButtons: Dialog.Close
        title: i18n("Change Timer Duration")

        contentItem: Kirigami.FormLayout {
            Layout.fillWidth: true

             RowLayout {
                 SpinBox {
                     id: spinBoxMinutes
                     onValueChanged: timerEditSheet.setDuration()
                     value: timerDuration / 60
                 }
                 Text {
                      color: Kirigami.Theme.textColor
                      text: i18n("minutes")
                 }
                 SpinBox {
                     id: spinBoxSeconds
                     to: 60
                     onValueChanged: timerEditSheet.setDuration()
                     value: timerDuration % 60
                 }
                 Text {
                      color: Kirigami.Theme.textColor
                      text: i18n("seconds")
                 }
             }
         }

         function setDuration() {
             timerDuration = spinBoxMinutes.value * 60 + spinBoxSeconds.value
         }
    }
}
