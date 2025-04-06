/*
 * Copyright 2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

import org.kde.kclock

Loader {
    id: loader
    active: false

    function open() {
        loader.active = false;
        AddLocationSearchModel.setFilterFixedString("");
        if (Kirigami.Settings.isMobile) {
            applicationWindow().pageStack.push(Qt.resolvedUrl("AddLocationPage.qml"));
        } else {
            loader.active = true;
            loader.item.open();
        }
    }

    sourceComponent: Kirigami.Dialog {
        id: dialog

        standardButtons: Kirigami.Dialog.NoButton
        parent: applicationWindow().overlay
        title: i18n("Add Location")
        preferredHeight: Kirigami.Units.gridUnit * 20
        preferredWidth: Kirigami.Units.gridUnit * 20
        padding: 0
        topPadding: 0
        bottomPadding: 0

        AddLocationListView {
            clip: true
            onCloseRequested: dialog.close()
        }

        footer: null
    }
}
