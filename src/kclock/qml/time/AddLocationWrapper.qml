/*
 * Copyright 2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.19 as Kirigami
import kclock 1.0
        
Loader {
    id: loader
    active: false
    
    function open() {
        loader.active = false;
        AddLocationSearchModel.setFilterFixedString("");
        if (Kirigami.Settings.isMobile) {
            applicationWindow().pageStack.push("qrc:/qml/time/AddLocationPage.qml");
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
        padding: Kirigami.Units.largeSpacing
        
        AddLocationListView {
            addPadding: false
            onCloseRequested: dialog.close()
        }
    }
}
