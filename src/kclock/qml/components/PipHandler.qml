/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

pragma Singleton

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.kclock as KClock

QtObject {
    id: pipHandler

    readonly property alias currentItem: stack.currentItem
    readonly property alias supported: window.supported

    function show(item, args) {
        stack.replace(item, args);
        window.show();
    }

    function hide() {
        stack.clear();
        window.hide();
    }

    property KClock.PipWindow pipWindow: KClock.PipWindow {
        id: window
        visible: !stack.empty

        Kirigami.ShadowedRectangle {
            anchors.fill: parent
            anchors.margins: 10
            // color: "salmon"
            shadow.size: 10
            radius: 15

            QQC2.StackView {
                id: stack
                anchors.fill: parent
                onCurrentItemChanged: {
                    if (currentItem) {
                        // TODO Binding?
                        window.width = currentItem.implicitWidth;
                        window.height = currentItem.implicitHeight;
                    }
                }
            }
        }
    }
}
