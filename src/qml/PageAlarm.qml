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

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.12 as Kirigami
import kirigamiclock 1.0

Kirigami.ScrollablePage {
    
    title: "Alarms"

    PageNewAlarm {
        id: pagenewalarm
        visible: false
    }

    mainAction: Kirigami.Action {
        iconName: "list-add"
        text: "New Alarm"
        onTriggered: {
            pagenewalarm.init(null);
            pageStack.push(pagenewalarm);
        }
    }

    ListView {
        model: alarmModel
        anchors.fill: parent

        delegate: Kirigami.SwipeListItem {

            onClicked: {
                pagenewalarm.init(alarmModel.get(index));
                pageStack.push(pagenewalarm);
            }
            
            actions: [
                Kirigami.Action {
                    iconName: "delete"
                    text: "Delete"
                    onTriggered: {
                        showPassiveNotification("Deleted alarm " + alarmModel.get(index).name);
                        alarmModel.remove(index);
                        alarmModel.updateUi();
                    }
                }
            ]

            contentItem: Item {
                implicitWidth: delegateLayout.implicitWidth
                implicitHeight: delegateLayout.implicitHeight

                GridLayout {
                    id: delegateLayout
                    anchors {
                        left: parent.left
                        top: parent.top
                        right: parent.right
                    }

                    rowSpacing: Kirigami.Units.smallSpacing
                    columnSpacing: Kirigami.Units.smallSpacing
                    columns: width > Kirigami.Units.gridUnit * 20 ? 4 : 2

                    ColumnLayout {
                        Kirigami.Heading {
                            level: 2
                            text: ("0" + model.hours).slice(-2) + ":" + ("0" + model.minutes).slice(-2)
                        }
                        Label {
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                            text: model.name
                        }
                    }

                    Switch {
                        Layout.alignment: Qt.AlignRight|Qt.AlignVCenter
                        Layout.columnSpan: 1
                        checked: model.enabled
                    }
                }
            }
        }
    }
}
