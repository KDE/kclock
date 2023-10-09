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

import kclock

ListView {
    id: root

    property string filterText: ""

    signal closeRequested()

    Component.onCompleted: root.filterText = "";

    currentIndex: -1
    reuseItems: true

    headerPositioning: ListView.OverlayHeader
    header: ToolBar {
        width: root.width
        z: 2

        contentItem: Kirigami.SearchField {
            id: searchField

            onTextChanged: {
                AddLocationSearchModel.setFilterFixedString(text);
                root.filterText = text;
                forceActiveFocus();
                focus = true;
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

        text: city

        onClicked: {
            AddLocationSearchModel.addLocation(index);
            ListView.view.closeRequested();
        }

        contentItem: RowLayout {
            spacing: Kirigami.Units.smallSpacing

            Delegates.SubtitleContentItem {
                itemDelegate: delegate
                subtitle: delegate.country
            }

            Label {
                text: delegate.currentTime
            }
        }
    }
}
