// SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes

import org.kde.kirigami as Kirigami

import org.kde.kclock

Kirigami.ScrollablePage {
    id: root

    title: i18n("Add Location")

    AddLocationListView {
        onCloseRequested: applicationWindow().pageStack.currentIndex = 0
    }
}
