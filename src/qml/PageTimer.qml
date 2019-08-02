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

import QtQuick 2.0
import QtQuick.Controls 2.0
import org.kde.kirigami 2.2 as Kirigami

Kirigami.Page {
    
    title: "Timer"
    id: timerpage
    property bool running: false
    property int timerDuration: 60
    property int elapsedTime: 0
    
    Label {
        text: (timerDuration - elapsedTime / 1000).toFixed(1)
        color: Kirigami.Theme.highlightColor
        font.pointSize: 40
    }

    Timer {
        interval: 100
        running: timerpage.running
        repeat: true
        onTriggered: {
            elapsedTime += interval
        }
    }
    
    mainAction: Kirigami.Action {
        iconName: running ? "media-playback-pause" : "media-playback-start"
        onTriggered: {
            running = !running
        }
    }

    leftAction: Kirigami.Action {
        iconName: "media-playback-stop"
        onTriggered: {
            elapsedTime = 0
            running = false
        }
    }

    Kirigami.OverlaySheet {
        id: timerEditSheet

        Column {
            Text {
                text: "Change timer duration"
            }
            Row {
                SpinBox {
                    id: spinBoxMinutes
                    onValueChanged: timerEditSheet.setDuration()
                    value: timerDuration / 60
                }

                SpinBox {
                    id: spinBoxSeconds
                    to: 60
                    onValueChanged: timerEditSheet.setDuration()
                    value: timerDuration % 60
                }
            }
        }

        function setDuration() {
            timerDuration = spinBoxMinutes.value * 60 + spinBoxSeconds.value
        }
    }

    rightAction: Kirigami.Action {
        onTriggered: {
            timerEditSheet.open()
        }
    }
}
