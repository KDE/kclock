/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import QtQuick.Window 2.11
import org.kde.kirigami 2.4 as Kirigami

Kirigami.ScrollablePage {
    id: stopwatchpage
    
    property int yTranslate
    
    title: i18n("Stopwatch")
    icon.name: "chronometer"
    
    property bool running: false
    property int elapsedTime: stopwatchTimer.elapsedTime
    
    Layout.fillWidth: true
    
    function toggleStopwatch() {
        running = !running;
        stopwatchTimer.toggle();
    }
    function addLap() {
        if (running) {
            if (roundModel.count === 0) {
                roundModel.append({ time: 0 }); // constantly counting lap
                roundModel.append({ time: elapsedTime });
            } else {
                roundModel.insert(0, { time: 0 }); // insert constantly count lap
                roundModel.get(1).time = elapsedTime;
            }
        }
    }
    function resetStopwatch() {
        running = false;
        roundModel.clear();
        stopwatchTimer.reset();
    }
    
    // keyboard controls
    Keys.onSpacePressed: toggleStopwatch();
    Keys.onReturnPressed: addLap();

    // start/pause button on mobile, reset button on desktop
    mainAction: Kirigami.Action {
        id: toggleAction
        iconName: !Kirigami.Settings.isMobile ? "chronometer-reset" : (running ? "chronometer-pause" : "chronometer-start")
        text: !Kirigami.Settings.isMobile ? i18n("Reset") : (running ? i18n("Pause") : i18n("Start"))
        onTriggered: !Kirigami.Settings.isMobile ? resetStopwatch() : toggleStopwatch()
    }
    
    header: ColumnLayout {
        transform: Translate { y: yTranslate }
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: Kirigami.Units.gridUnit

        // clock display
        Item {
            Layout.topMargin: Kirigami.Units.gridUnit
            Layout.alignment: Qt.AlignHCenter
            width: timeLabels.implicitWidth
            height: timeLabels.implicitHeight
            
            MouseArea {
                anchors.fill: timeLabels
                onClicked: toggleStopwatch()
            }
            
            Row {
                id: timeLabels
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: Math.round(Kirigami.Units.smallSpacing / 2)

                Label {
                    id: text
                    text: stopwatchTimer.minutes + ':' + stopwatchTimer.seconds + '.'
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 4
                    font.weight: Font.Light
                }
                Label {
                    id: secondsText
                    anchors.baseline: text.baseline
                    text: stopwatchTimer.small
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 3
                    font.weight: Font.Light
                }
            }
        }

        // reset button on mobile, start/pause on desktop, and lap button
        RowLayout {
            id: buttons
            Layout.fillWidth: true
            Layout.bottomMargin: Kirigami.Units.gridUnit
            
            Item { Layout.fillWidth: true }
            Button {
                implicitHeight: Kirigami.Units.gridUnit * 2
                implicitWidth: Kirigami.Units.gridUnit * 6
                Layout.alignment: Qt.AlignHCenter
                
                icon.name: Kirigami.Settings.isMobile ? "chronometer-reset" : (running ? "chronometer-pause" : "chronometer-start")
                text: Kirigami.Settings.isMobile ? i18n("Reset") : (running ? i18n("Pause") : i18n("Start"))
                
                onClicked: {
                    if (Kirigami.Settings.isMobile) {
                        resetStopwatch();
                    } else {
                        toggleStopwatch();
                    }
                    focus = false; // prevent highlight
                }
            }
            Item { Layout.fillWidth: true }
            Button {
                implicitHeight: Kirigami.Units.gridUnit * 2
                implicitWidth: Kirigami.Units.gridUnit * 6
                Layout.alignment: Qt.AlignHCenter
                
                icon.name: "chronometer-lap"
                text: i18n("Lap")
                enabled: running
                
                onClicked: {
                    addLap();
                    focus = false; // prevent highlight
                }
            }
            Item { Layout.fillWidth: true }
        }
    }
    
    // lap list display
    ListView {
        id: listView
        model: roundModel
        spacing: 0
        currentIndex: -1
        transform: Translate { y: yTranslate }

        reuseItems: true
        
        ListModel {
            id: roundModel
        }
        
        remove: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: Kirigami.Units.shortDuration }
        }
        displaced: Transition {
            NumberAnimation { properties: "x,y"; duration: Kirigami.Units.longDuration; easing.type: Easing.InOutQuad}
        }
        
        // lap items
        delegate: Kirigami.BasicListItem {
            id: listItem
            
            y: -height
            leftPadding: Kirigami.Units.largeSpacing * 2
            rightPadding: Kirigami.Units.largeSpacing * 2
            topPadding: Kirigami.Units.largeSpacing
            bottomPadding: Kirigami.Units.largeSpacing
            activeBackgroundColor: "transparent"

            opacity: 0
            ListView.onReused: opacityAnimation.restart()
            Component.onCompleted: opacityAnimation.restart()
            NumberAnimation on opacity {
                id: opacityAnimation
                duration: Kirigami.Units.shortDuration
                from: 0
                to: 1
            }
            
            Keys.onSpacePressed: toggleStopwatch()
            
            property int lapNumber: model.index == -1 ? -1 : roundModel.count - model.index
            
            property double timeSinceLastLap: {
                if (index === 0) { // constantly updated lap (top lap)
                    return parseFloat((elapsedTime - roundModel.get(1).time)/1000)
                } else if (index === roundModel.count - 1) { // last lap
                    return parseFloat(model.time / 1000)
                } else if (model) {
                    return parseFloat((model.time - roundModel.get(index+1).time)/1000)
                }
            }
            
            property double timeSinceBeginning: parseFloat((index == 0 ? elapsedTime : model.time) / 1000)
            
            contentItem: RowLayout {
                Item { Layout.fillWidth: true }
                
                RowLayout {
                    Layout.maximumWidth: Kirigami.Units.gridUnit * 16
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 16
                    
                    // lap number
                    Item {
                        Layout.fillHeight: true
                        Layout.leftMargin: Kirigami.Units.largeSpacing
                        Layout.minimumWidth: Kirigami.Units.gridUnit * 2
                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            color: Kirigami.Theme.textColor
                            font.weight: Font.Bold
                            text: listItem.lapNumber >= 0 ? i18n("#%1", listItem.lapNumber) : ""
                        }
                    }
                    
                    // time since last lap
                    Label {
                        Layout.alignment: Qt.AlignLeft
                        color: Kirigami.Theme.textColor
                        text: isNaN(timeSinceLastLap) ? "" : "+" + listItem.timeSinceLastLap.toFixed(2);
                    }
                    
                    Item { Layout.fillWidth: true }
                    
                    // time since beginning
                    Item {
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignRight
                        Layout.minimumWidth: Kirigami.Units.gridUnit * 3
                        Label {
                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            color: Kirigami.Theme.focusColor
                            text: isNaN(timeSinceBeginning) ? "" : listItem.timeSinceBeginning.toFixed(2) 
                        }
                    }
                }
                
                Item { Layout.fillWidth: true }
            }
        }
    }
}
