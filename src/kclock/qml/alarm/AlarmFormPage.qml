/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 * Copyright 2019 Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.3

import org.kde.kirigami 2.11 as Kirigami
import org.kde.kirigamiaddons.dateandtime 0.1 as DateAndTime

import "../components"
import kclock 1.0

Kirigami.ScrollablePage {
    id: root

    // null if this is a new alarm page, and the alarm to edit otherwise
    property Alarm selectedAlarm: null
    
    title: selectedAlarm ? i18nc("Edit alarm page title", "Editing %1", selectedAlarm.name === "" ? i18n("Alarm") : selectedAlarm.name)
                         : i18n("New Alarm");
    
    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false
    
    function accept() {
        form.submitForm();
        pageStack.layers.pop();
    }
    
    actions.main: Kirigami.Action {
        iconName: "dialog-ok"
        text: i18n("Done")
        onTriggered: root.accept()
    }
    
    // mobile footer actions
    footer: ToolBar {
        id: toolbar
        visible: Kirigami.Settings.isMobile
        height: Kirigami.Settings.isMobile ? implicitHeight : 0
        topPadding: 0; bottomPadding: 0
        rightPadding: 0; leftPadding: 0
        
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false
        
        property bool opened: false
        RowLayout {
            anchors.fill: parent
            spacing: 0
            
            Item { Layout.fillWidth: true }
            FooterToolBarButton {
                display: toolbar.opened ? AbstractButton.TextUnderIcon : AbstractButton.TextOnly
                text: i18n("Done")
                icon.name: "dialog-ok"
                onClicked: root.accept()
            }
            FooterToolBarButton {
                display: toolbar.opened ? AbstractButton.TextUnderIcon : AbstractButton.TextOnly
                text: i18n("Cancel")
                icon.name: "dialog-cancel"
                onClicked: applicationWindow().pageStack.layers.pop()
            }
            FooterToolBarButton {
                display: toolbar.opened ? AbstractButton.TextUnderIcon : AbstractButton.TextOnly
                icon.name: "view-more-symbolic"
                onClicked: toolbar.opened = !toolbar.opened
                iconSize: Kirigami.Units.iconSizes.small
                implicitWidth: Kirigami.Units.gridUnit * 2.5
            }
        }
    }

    ColumnLayout {
        AlarmForm {
            id: form
            Layout.fillWidth: true
            Layout.maximumWidth: root.width - root.leftPadding - root.rightPadding
            selectedAlarm: root.selectedAlarm
        }
    }
}

