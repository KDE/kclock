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
import kclock 1.0

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
            pagenewalarm.init(null, null);
            pageStack.push(pagenewalarm);
        }
    }
    
    function getTimeFormat(hours, minutes) {
        if (settings.use24HourTime) {
            return ("0" + hours).slice(-2) + ":" + ("0" + minutes).slice(-2)
        } else {
            if (hours >= 12) { // pm
                if (hours - 12 == 0) hours = 24;
                return ("0" + (hours - 12)).slice(-2) + ":" + ("0" + minutes).slice(-2) + " PM";
            } else { // am
                if (hours == 0) hours = 12;
                return ("0" + hours).slice(-2) + ":" + ("0" + minutes).slice(-2) + " AM";
            }
        }
    }
    
    function getRepeatFormat(dayOfWeek) {
        if (dayOfWeek == 0) {
            return qsTr("Only once");
        }
        let sunday = 1 << 0, monday = 1 << 1, tuesday = 1 << 2, wednesday = 1 << 3, thursday = 1 << 4, friday = 1 << 5, saturday = 1 << 6;
        if (dayOfWeek == sunday + monday + tuesday + wednesday + thursday + friday + saturday)
            return qsTr("Everyday");
        
        let str = "";
        if (dayOfWeek & sunday) str += "Sun., ";
        if (dayOfWeek & monday) str += "Mon., ";
        if (dayOfWeek & tuesday) str += "Tue., ";
        if (dayOfWeek & wednesday) str += "Wed., ";
        if (dayOfWeek & thursday) str += "Thu., ";
        if (dayOfWeek & friday) str += "Fri., ";
        if (dayOfWeek & saturday) str += "Sat., ";
        return str.substring(0, str.length - 2);
    }

    ListView {
        id: alarmsList
        model: alarmModel

        // no alarms placeholder
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: Kirigami.Units.largeSpacing
            visible: alarmsList.count == 0
            text: "No alarms configured"
            icon.name: "notifications"
        }
        
        // each alarm
        delegate: Kirigami.SwipeListItem {

            onClicked: {
                model.enabled = !model.enabled
            }
            
            actions: [
                Kirigami.Action {
                    iconName: "entry-edit"
                    text: "Edit"
                    onTriggered: {
                        pagenewalarm.init(alarmModel.get(index), model);
                        pageStack.push(pagenewalarm);
                    }
                },
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

            // alarm text
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
                        
                        Label {
                            font.weight: Font.Light
                            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
                            text: getTimeFormat(model.hours, model.minutes)
                        }
                        Label {
                            font.weight: Font.Bold
                            color: model.enabled ? Kirigami.Theme.activeTextColor : Kirigami.Theme.disabledTextColor
                            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.15
                            text: model.name
                        }
                        Label {
                            font.weight: Font.Normal
                            text: getRepeatFormat(model.dayOfWeek)
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
