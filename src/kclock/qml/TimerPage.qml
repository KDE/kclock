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
    title: timer && timer.label !== "" ? timer.label : i18n("New timer")
    
    property bool showFullscreen: false
    
    property int elapsed: timer == null ? 0 : timer.elapsed
    property int duration: timer == null ? 0 : timer.length
    property bool running: timer == null ? 0 : timer.running

    // keyboard controls
    Keys.onSpacePressed: timer.toggleRunning();
    Keys.onReturnPressed: timer.toggleRunning();
    
    // topbar action
    actions {
        main: Kirigami.Action {
            text: running ? i18n("Pause") : i18n("Start")
            iconName: running ? "chronometer-pause" : "chronometer-start"
            onTriggered: timer.toggleRunning()
        }

        contextualActions: [
            Kirigami.Action {
                icon.name: "chronometer-reset"
                text: i18n("Reset")
                onTriggered: timer.reset();
            },
            Kirigami.Action {
                icon.name: "delete"
                text: i18n("Delete")
                onTriggered: {
                    pageStack.pop();
                    timerModel.remove(timerIndex);
                }
            }
        ]
    }

    TimerComponent {
        timerDuration: duration
        timerElapsed: elapsed
        timerRunning: running
    }
}
