/*
 * Copyright 2021 Devin Lin <devin@kde.org>
 * Copyright 2023 Nate Graham <nate@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.kclock

ListView {
    id: root

    signal closeRequested()

    currentIndex: -1
    reuseItems: true

    headerPositioning: ListView.OverlayHeader
    header: ToolBar {
        width: root.width
        z: 2

        contentItem: Kirigami.SearchField {
            id: searchField

            Component.onCompleted: forceActiveFocus(Qt.PopupFocusReason)

            onTextChanged: {
                forceActiveFocus();
                focus = true;
            }

            Binding {
                target: AddLocationSearchModel
                property: "query"
                value: searchField.text
            }
        }
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        visible: root.count == 0
        text: i18n("No locations found")
        icon.name: "globe"
    }

    model: AddLocationSearchModel

    delegate: Delegates.RoundedItemDelegate {
        id: delegate

        required property int index
        required property string city
        required property string country
        required property string currentTime
        required property bool added

        text: city
        enabled: !added
        // TODO Fix RoundedItemDelegate to not highlight disabled entries.
        highlighted: enabled && delegate.ListView.isCurrentItem
        hoverEnabled: enabled && !Kirigami.Settings.isMobile

        onClicked: {
            AddLocationSearchModel.addLocation(index);
            ListView.view.closeRequested();
        }

        contentItem: RowLayout {
            spacing: Kirigami.Units.smallSpacing

            Delegates.SubtitleContentItem {
                itemDelegate: delegate
                subtitle: delegate.country
                Layout.fillWidth: true
            }

            Kirigami.Icon {
                Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
                Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
                source: delegate.added ? "dialog-ok-symbolic" : ""
                visible: valid
            }

            Label {
                text: delegate.currentTime
            }
        }
    }
}
