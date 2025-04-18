/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

import org.kde.kirigami as Kirigami

import org.kde.kclock

Kirigami.ScrollablePage {
    id: root

    property real yTranslate

    objectName: "Stopwatch"
    title: i18n("Stopwatch")
    readonly property string hiddenTitle: running ? StopwatchTimer.display : ""
    icon.name: "chronometer"
    background: null

    readonly property bool running: !StopwatchTimer.stopped && !StopwatchTimer.paused
    readonly property int elapsedTime: StopwatchTimer.elapsedTime

    // keyboard controls
    Keys.onSpacePressed: StopwatchTimer.toggle();
    Keys.onReturnPressed: {
        if (running) {
            StopwatchModel.addLap();
        }
    }
    Keys.onDeletePressed: {
        if (!running) {
            StopwatchTimer.reset();
        }
    }
    Keys.onPressed: (event) => {
        if (event.key === Qt.Key_Backspace && !running) {
            StopwatchTimer.reset();
            event.accepted = true;
        }
    }

    actions: [
        Kirigami.Action {
            displayHint: Kirigami.DisplayHint.IconOnly
            visible: !applicationWindow().isWidescreen
            icon.name: "settings-configure"
            text: i18n("Settings")
            onTriggered: applicationWindow().pageStack.push(applicationWindow().getPage("Settings"))
        }
    ]

    header: ColumnLayout {
        transform: Translate { y: root.yTranslate }
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 0

        // clock display
        Item {
            Layout.fillWidth: true
            height: timeLabels.implicitHeight + lapText.implicitHeight

            Column {
                anchors.horizontalCenter: parent.horizontalCenter
                y: Math.round((parent.height - height) / 2)
                Behavior on y {
                    NumberAnimation { duration: Kirigami.Units.longDuration; easing.type: Easing.InOutQuad }
                }

                // toggle when clicked
                MouseArea {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: timeLabels.implicitWidth
                    height: timeLabels.implicitHeight
                    onClicked: StopwatchTimer.toggle()

                    Row {
                        id: timeLabels
                        spacing: Math.round(Kirigami.Units.smallSpacing / 2)

                        Label {
                            id: text
                            text: StopwatchTimer.display + "."
                            font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 4.75)
                            font.weight: Font.ExtraLight
                        }
                        Label {
                            id: secondsText
                            anchors.baseline: text.baseline
                            text: StopwatchTimer.small
                            font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 3.75)
                            font.weight: Font.ExtraLight
                        }
                    }
                }

                // elapsed duration for a lap
                Label {
                    id: lapText
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: listView.count > 0
                    opacity: 0.8

                    font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 1.1)
                    font.weight: Font.Bold

                    text: {
                        const duration = StopwatchTimer.elapsedTime - StopwatchModel.mostRecentLapTime;

                        const hours = UtilModel.displayTwoDigits(UtilModel.msToHoursPart(duration));
                        const minutes = UtilModel.displayTwoDigits(UtilModel.msToMinutesPart(duration));
                        const seconds = UtilModel.displayTwoDigits(UtilModel.msToSecondsPart(duration));
                        const small = UtilModel.displayTwoDigits(UtilModel.msToSmallPart(duration));

                        // only show hours if we have passed an hour
                        if (hours === '00') {
                            return "%1:%2.%3".arg(minutes).arg(seconds).arg(small);
                        } else {
                            return "%1:%2:%3.%4".arg(hours).arg(minutes).arg(seconds).arg(small);
                        }
                    }
                }
            }
        }

        // reset button on mobile, start/pause on desktop, and lap button
        RowLayout {
            id: buttons
            Layout.topMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true

            Item { Layout.fillWidth: true }

            Item {
                Layout.preferredWidth: Kirigami.Units.gridUnit * 6
                Layout.preferredHeight: startButton.implicitHeight

                // on mobile -> reset button
                // otherwise -> start/stop button
                Button {
                    id: startButton
                    anchors.centerIn: parent
                    width: Math.max(Kirigami.Units.gridUnit * 6, implicitWidth)

                    icon.name: Kirigami.Settings.isMobile ? "chronometer-reset" : (root.running ? "chronometer-pause" : "chronometer-start")
                    text: {
                        if (Kirigami.Settings.isMobile) {
                            return i18n("Reset");
                        } else if (root.running) {
                            return i18n("Pause");
                        } else if (StopwatchTimer.paused) {
                            return i18n("Resume")
                        } else {
                            return i18n("Start");
                        }
                    }

                    onClicked: {
                        if (Kirigami.Settings.isMobile) {
                            StopwatchTimer.reset();
                        } else {
                            StopwatchTimer.toggle();
                        }
                        focus = false; // prevent highlight
                    }
                }
            }

            Item { Layout.fillWidth: true }

            Item {
                Layout.preferredWidth: Kirigami.Units.gridUnit * 6
                Layout.preferredHeight: resetButton.implicitHeight

                // on desktop & paused -> reset button
                // otherwise -> lap button
                Button {
                    id: resetButton
                    anchors.centerIn: parent
                    width: Math.max(Kirigami.Units.gridUnit * 6, implicitWidth)

                    icon.name: (!Kirigami.Settings.isMobile && !root.running) ? "chronometer-reset" : "chronometer-lap";
                    text: (!Kirigami.Settings.isMobile && !root.running) ? i18n("Reset") : i18n("Lap");

                    onClicked: {
                        if (!Kirigami.Settings.isMobile && !root.running) {
                            StopwatchTimer.reset();
                        } else {
                            StopwatchModel.addLap();
                        }
                        focus = false; // prevent highlight
                    }
                }
            }

            Item { Layout.fillWidth: true }
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: Kirigami.Units.gridUnit * 18
            Layout.topMargin: Kirigami.Units.gridUnit
            horizontalAlignment: Text.AlignHCenter
            text: i18n("Press Space to start or pause the stopwatch. Delete or Backspace resets it. Enter adds a lap.")
            color: Kirigami.Theme.disabledTextColor
            font: Kirigami.Theme.smallFont
            wrapMode: Text.WordWrap
            visible: !Kirigami.Settings.isMobile && listView.count === 0
        }

        // laps list header 
        RowLayout {
            Layout.topMargin: Kirigami.Units.gridUnit
            Layout.bottomMargin: Kirigami.Units.largeSpacing
            visible: listView.count > 0

            Item { Layout.fillWidth: true }
            RowLayout {
                Layout.maximumWidth: Kirigami.Units.gridUnit * 16
                Layout.preferredWidth: Kirigami.Units.gridUnit * 16

                Label {
                    horizontalAlignment: Text.AlignLeft
                    text: i18n("Lap")
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    font.bold: true
                }

                Label {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: i18n("Lap Time")
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    font.bold: true
                }

                Label {
                    horizontalAlignment: Text.AlignRight
                    text: i18n("Total")
                    font.pointSize: Kirigami.Theme.smallFont.pointSize
                    font.bold: true
                }
            }
            Item { Layout.fillWidth: true }
        }
    }

    // lap list display
    ListView {
        id: listView
        model: StopwatchModel
        spacing: 0
        currentIndex: -1
        transform: Translate { y: root.yTranslate }

        reuseItems: true

        remove: Transition {
            NumberAnimation { property: "opacity"; from: 1; to: 0; duration: Kirigami.Units.shortDuration }
        }
        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: Kirigami.Units.longDuration; easing.type: Easing.InOutQuad}
        }

        // mobile action
        FloatingActionButton {
            text: {
                if (root.running) {
                    return i18nc("@action:button", "Pause");
                } else if (StopwatchTimer.paused) {
                    return i18nc("@action:button", "Resume");
                } else {
                    return i18nc("@action:button", "Start");
                }
            }
            icon.name: root.running ? "chronometer-pause" : "chronometer-start"
            visible: Kirigami.Settings.isMobile
            onClicked: StopwatchTimer.toggle()
        }

        // lap items
        delegate: ItemDelegate {
            id: listItem

            required property int lapNumber
            required property double lapTime
            required property double lapTimeSinceBeginning
            required property bool isBest
            required property bool isWorst

            background: null
            width: ListView.view.width

            ListView.onReused: opacityAnimation.restart()
            Component.onCompleted: opacityAnimation.restart()
            Keys.onSpacePressed: StopwatchTimer.toggle()

            NumberAnimation on opacity {
                id: opacityAnimation
                duration: Kirigami.Units.shortDuration
                from: 0
                to: 1
            }

            contentItem: RowLayout {
                Item { Layout.fillWidth: true }

                RowLayout {
                    Layout.maximumWidth: Kirigami.Units.gridUnit * 16
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 16

                    Kirigami.Icon {
                        implicitHeight: Kirigami.Units.iconSizes.small
                        implicitWidth: Kirigami.Units.iconSizes.small
                        source: {
                            if (listItem.isBest && listItem.isWorst) {
                                return 'flag-blue';
                            } else if (listItem.isBest) {
                                return 'flag-green';
                            } else if (listItem.isWorst) {
                                return 'flag-red';
                            }
                            return 'flag-blue';
                        }
                        Layout.alignment: Qt.AlignBottom
                    }

                    // lap number
                    Label {
                        id: lapLabel
                        Layout.leftMargin: Kirigami.Units.largeSpacing
                        font.weight: Font.Bold
                        text: listItem.lapNumber >= 0 ? i18n("#%1", listItem.lapNumber) : ""
                    }

                    // time since last lap
                    Label {
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                        text: {
                            if (isNaN(listItem.lapTime)) {
                                return ""; 
                            }

                            const duration = listItem.lapTime;
                            const hours = UtilModel.displayTwoDigits(UtilModel.msToHoursPart(duration));
                            const minutes = UtilModel.displayTwoDigits(UtilModel.msToMinutesPart(duration));
                            const seconds = UtilModel.displayTwoDigits(UtilModel.msToSecondsPart(duration));
                            const small = UtilModel.displayTwoDigits(UtilModel.msToSmallPart(duration));

                            // only show hours if we have passed an hour
                            if (hours === '00') {
                                return "+%1:%2.%3".arg(minutes).arg(seconds).arg(small);
                            } else {
                                return "+%1:%2:%3.%4".arg(hours).arg(minutes).arg(seconds).arg(small);
                            }
                        }
                    }

                    // time since beginning
                    Label {
                        horizontalAlignment: Text.AlignRight
                        color: Kirigami.Theme.focusColor
                        text: {
                            if (isNaN(listItem.lapTimeSinceBeginning)) {
                                return ""; 
                            }

                            const duration = listItem.lapTimeSinceBeginning;
                            const hours = UtilModel.displayTwoDigits(UtilModel.msToHoursPart(duration));
                            const minutes = UtilModel.displayTwoDigits(UtilModel.msToMinutesPart(duration));
                            const seconds = UtilModel.displayTwoDigits(UtilModel.msToSecondsPart(duration));
                            const small = UtilModel.displayTwoDigits(UtilModel.msToSmallPart(duration));

                            // only show hours if we have passed an hour
                            if (hours === '00') {
                                return "%1:%2.%3".arg(minutes).arg(seconds).arg(small);
                            } else {
                                return "%1:%2:%3.%4".arg(hours).arg(minutes).arg(seconds).arg(small);
                            }
                        }
                    }
                }

                Item { Layout.fillWidth: true }
            }
        }
    }
}
