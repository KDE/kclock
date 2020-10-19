
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
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.12 as Kirigami
import kclock 1.0

Kirigami.ScrollablePage {
    
    title: i18n("Timers")
    icon.name: "player-time"
    property bool createdTimer: false
    
    mainAction: Kirigami.Action {
        iconName: "list-add"
        text: i18n("New Timer")
        onTriggered: {
            createdTimer = true;
            timerModel.addNew();
        }
    }
    
    Kirigami.CardsListView {
        id: timersList
        model: timerModel
        
        // no timer placeholder
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: timersList.count === 0
            text: i18n("Add a timer")
            icon.name: "player-time"
        }
        
        // each timer
        delegate: Kirigami.AbstractCard {
            
            property Timer timerDelegate: timerModel.get(index)
            
            TimerPage {
                id: timerPage
                timerIndex: index
                timer: timerDelegate
                visible: false
            }
            
            Component.onCompleted: {
                if (createdTimer)
                    switchToPage(timerPage, 1)
            }
            
            showClickFeedback: true
            onClicked: switchToPage(timerPage, 1)
                
            contentItem: Item {
                implicitWidth: delegateLayout.implicitWidth
                implicitHeight: delegateLayout.implicitHeight
                
                Column {
                    id: delegateLayout
                    anchors {
                        left: parent.left
                        top: parent.top
                        right: parent.right
                    }
                    
                    spacing: Kirigami.Units.smallSpacing
                    
                    ProgressBar {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        property real progress: timerDelegate.elapsed / timerDelegate.length
                        value: 0
                        Component.onCompleted: value = progress
                        
                        onProgressChanged: {
                            progressTransition.to = progress;
                            progressTransition.restart();
                        }
                        NumberAnimation on value {
                            id: progressTransition
                            duration: 300
                            easing.type: Easing.InOutQuad
                        }
                    }
                    
                    RowLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        Kirigami.Heading {
                            level: 3
                            Layout.alignment: Qt.AlignLeft
                            text: timerDelegate.elapsedPretty
                            color: timerDelegate.running ? Kirigami.Theme.activeTextColor : Kirigami.Theme.disabledTextColor
                        }
                        Kirigami.Heading {
                            level: 3
                            Layout.alignment: Qt.AlignRight
                            text: timerDelegate.lengthPretty
                            color: timerDelegate.running ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                        }
                    }
                    
                    RowLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        Kirigami.Heading {
                            level: 4
                            Layout.alignment: Qt.AlignLeft
                            text: timerDelegate.label
                        }
                        
                        Row {
                            Layout.alignment: Qt.AlignRight
                            
                            ToolButton {
                                icon.name: timerDelegate.running ? "chronometer-pause" : "chronometer-start"
                                onClicked: timerDelegate.toggleRunning()
                            }
                            ToolButton {
                                icon.name: "chronometer-reset"
                                onClicked: timerDelegate.reset();
                            }
                            ToolButton {
                                icon.name: "delete"
                                onClicked: timerModel.remove(index)
                            }
                        }
                    }
                }
            }
        }
    }
}
