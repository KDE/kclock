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
    sourceComponent: Kirigami.Settings.isMobile ? mobileComponent : desktopComponent
    
    function open() {
        loader.active = false;
        loader.active = true;
        if (Kirigami.Settings.isMobile) {
            applicationWindow().pageStack.push(loader.item);
        } else {
            loader.item.open();
        }
    }
    
    Component {
        id: mobileComponent
        Kirigami.ScrollablePage {
            id: root
            
            title: i18n("Add Location")
            
            AddLocationListView {
                addPadding: true
                onCloseRequested: applicationWindow().pageStack.currentIndex = 0
            }
        }
    }
    
    Component {
        id: desktopComponent
        Kirigami.Dialog {
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
} 
