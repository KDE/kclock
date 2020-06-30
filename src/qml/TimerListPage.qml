
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
    
    title: "Timers"
    
    TimerPage {
        id: timerPage
    }
    
    mainAction: Kirigami.Action {
        iconName: "list-add"
        text: "New Timer"
        onTriggered: {
            timerPage.addNew();
            timerPage.timer = timerModel.at(timerModel.count()-1);
        }
    }
    
    Kirigami.CardsListView {
        id: timersList
        model: timerModel
        
        // no timer placeholder
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: Kirigami.Units.largeSpacing
            visible: timersList.count == 0
            text: i18n("Add a timer")
            icon.name: "player-time"
        }
        
        // each timer
        delegate: Kirigami.AbstractCard {
            
            property Timer timer: timerModel.get(index)
            
            contentItem: Item {
                implicitWidth: delegateLayout.implicitWidth
                implicitHeight: delegateLayout.implicitHeight
                
                ColumnLayout {
                    anchors {
                        left: parent.left
                        top: parent.top
                        right: parent.right
                    }
                    
                    spacing: Kirigami.Units.largeSpacing
                    
                    ProgressBar {
                        Layout.fillWidth: true
                        value: 0.5
                        enabled: true
                    }
                    
                    Row {
                        Layout.fillWidth: true
                        Label {
                            Layout.alignment: Qt.AlignLeft
                            text: "0:30:00"
                        }
                        Label {
                            Layout.alignment: Qt.AlignRight
                            text: "1:00:00"
                        }
                    }
                    
                    Label {
                        text: "Default Timer"
                    }
                }
            }
        }
    }
}
