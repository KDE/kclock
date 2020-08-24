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
        Layout.preferredWidth: alarmLabel.width
        MouseArea {
            anchors.fill: parent
            onClicked: plasmoid.nativeInterface.openKClock()
        }
        ColumnLayout {
            spacing: 0
            PlasmaComponents.Label {
                text: plasmoid.nativeInterface.time
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 3
                color: "white"
                Layout.alignment: Qt.AlignHCenter
            }
            RowLayout {
                id: alarmLabel
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
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
                }
            }
        }
    }
    Plasmoid.compactRepresentation: Kirigami.Icon {
        source: "notifications"
        height: Kirigami.Units.gridUnit
        width: Kirigami.Units.gridUnit
    }
    Plasmoid.status: plasmoid.nativeInterface.hasAlarm ? PlasmaCore.Types.ActiveStatus : PlasmaCore.Types.Hidden
}
