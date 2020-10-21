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
    
    title: i18n("Alarms")
    icon.name: "notifications"
    mainAction: Kirigami.Action {
        iconName: "list-add"
        text: "New Alarm"
        onTriggered: appwindow.pageStack.layers.push(Qt.resolvedUrl("NewAlarmPage.qml"))
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
            text: i18n("No alarms configured")
            icon.name: "notifications"
        }
        
        // each alarm
        delegate: Kirigami.SwipeListItem {

            onClicked: model.enabled = !model.enabled
            
            actions: [
                Kirigami.Action {
                    iconName: "entry-edit"
                    text: i18n("Edit")
                    onTriggered: pageStack.layers.push(Qt.resolvedUrl("NewAlarmPage.qml"), {selectedAlarm: model.alarm})
                },
                Kirigami.Action {
                    iconName: "delete"
                    text: i18n("Delete")
                    onTriggered: {
                        showPassiveNotification(i18n("Deleted %1", model.name));
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
                            text: kclockFormat.formatTimeString(model.hours, model.minutes)
                        }
                        Label {
                            visible: model.alarm.snoozedMinutes != 0
                            font.weight: Font.Bold
                            color: Kirigami.Theme.disabledTextColor
                            text: i18n("Snoozed ") + model.alarm.snoozedMinutes + i18n(" minutes")
                        }
                        Label {
                            id: alarmName
                            font.weight: Font.Bold
                            color: model.enabled ? Kirigami.Theme.activeTextColor : Kirigami.Theme.disabledTextColor
                            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.15
                            text: model.name
                        }
                        Label {
                            font.weight: Font.Normal
                            text: getRepeatFormat(model.daysOfWeek) // related to UI improvements, leave it for now
                        }
                    }

                    Switch {
                        Layout.alignment: Qt.AlignRight|Qt.AlignVCenter
                        Layout.columnSpan: 1
                        checked: model.enabled
                        onCheckedChanged: {
                            model.enabled = checked;
                            alarmName.color = checked ? Kirigami.Theme.activeTextColor : Kirigami.Theme.disabledTextColor;
                        }
                    }
                }
            }
        }
    }

    function getRepeatFormat(dayOfWeek) {
        if (dayOfWeek == 0) {
            return i18n("Only once");
        }
        let monday = 1 << 0, tuesday = 1 << 1, wednesday = 1 << 2, thursday = 1 << 3, friday = 1 << 4, saturday = 1 << 5, sunday = 1 << 6;

        if (dayOfWeek == monday + tuesday + wednesday + thursday + friday + saturday + sunday)
            return i18n("Everyday");

        if (dayOfWeek == monday + tuesday + wednesday + thursday + friday)
            return i18n("Weekdays");

        let str = "";
        if (dayOfWeek & monday) str += "Mon., ";
        if (dayOfWeek & tuesday) str += "Tue., ";
        if (dayOfWeek & wednesday) str += "Wed., ";
        if (dayOfWeek & thursday) str += "Thu., ";
        if (dayOfWeek & friday) str += "Fri., ";
        if (dayOfWeek & saturday) str += "Sat., ";
        if (dayOfWeek & sunday) str += "Sun., ";
        return str.substring(0, str.length - 2);
    }
}
