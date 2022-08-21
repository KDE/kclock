// SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import QtQuick.Shapes 1.12

import org.kde.kirigami 2.15 as Kirigami

import "../components"
import kclock 1.0

Kirigami.ScrollablePage {
    id: root
    
    title: i18n("Add Location")
    
    AddLocationListView {
        addPadding: true
        onCloseRequested: applicationWindow().pageStack.currentIndex = 0
    }
}
