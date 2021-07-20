/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
    property bool isCommandTimeout: timer == null ? 0 : timer.isCommandTimeout

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
            },
            Kirigami.Action {
                icon.name: "dialog-scripts"
                text: i18n("Command")
                checkable: true
                checked: isCommandTimeout
                onTriggered: timer.toggleIsCommandTimeout();
            }
        ]
    }

    TimerComponent {
        timerDuration: duration
        timerElapsed: elapsed
        timerRunning: running
    }
}
