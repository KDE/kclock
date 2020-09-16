/*
    SPDX-FileCopyrightText: 2020 HanY <hanyoung@protonmail.com>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick 2.1
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.kirigami 2.11 as Kirigami

Item {
    Plasmoid.backgroundHints: "ShadowBackground";
    Plasmoid.fullRepresentation: Item {
        property int fontSize: mainItem.height / 4
        id: mainItem
        Layout.preferredHeight: plasmoid.nativeInterface.hasAlarm ? Kirigami.Theme.defaultFont.pointSize * 8 : Kirigami.Theme.defaultFont.pointSize * 16
        Layout.preferredWidth: Kirigami.Units.gridUnit * 20
        Layout.alignment: Qt.AlignHCenter
        MouseArea {
            anchors.fill: parent
            onClicked: plasmoid.nativeInterface.openKClock()
        }
        ColumnLayout {
            id: mainDisplay
            spacing:0
            PlasmaComponents.Label {
                text: plasmoid.nativeInterface.time
                font.pointSize: fontSize
                color: "white"
                Layout.alignment: Qt.AlignHCenter
            }
            RowLayout {
                visible: plasmoid.nativeInterface.hasAlarm
                Layout.alignment: Qt.AlignHCenter
                Kirigami.Icon {
                    source: "notifications"
                    Layout.preferredHeight: alarmTime.height
                    Layout.preferredWidth: alarmTime.height
                }
                PlasmaComponents.Label {
                    id: alarmTime
                    Layout.alignment: Qt.AlignCenter
                    text: plasmoid.nativeInterface.alarmTime
                    color: "white"
                    font.pointSize: fontSize / 2
                }
            }
        }
    }
    Plasmoid.status : plasmoid.nativeInterface.hasAlarm ? PlasmaCore.Types.ActiveStatus : PlasmaCore.Types.Hidden
}
