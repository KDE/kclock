/*
 * Copyright 2025 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Templates as T
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

StackView {
    id: stackView
    anchors.fill: parent
    focus: true

    Shortcut {
        sequences: [ StandardKey.Back ]
        onActivated: {
            if (root.pageStack.depth > 1) {
                root.pageStack.pop();
            }
        }
    }

    // Drag gesture to dismiss page
    DragHandler {
        enabled: stackView.currentItem && stackView.currentItem.StackView.index > 0
        target: stackView.currentItem
        yAxis.enabled: false
        xAxis.enabled: true
        xAxis.minimum: 0
        xAxis.maximum: stackView.width

        property bool closing: false
        onActiveTranslationChanged: (t) => {
            closing = t.x > 0;
        }
        onActiveChanged: {
            if (active) {
                xAnim.stop();

                // By default, StackView hides elements below
                if (stackView.depth > 1) {
                    stackView.get(stackView.depth - 2).StackView.visible = true;
                }
            } else  {
                if (closing) {
                    stackView.pop();
                } else {
                    xAnim.to = 0;
                    xAnim.restart();
                }
            }
        }
    }

    NumberAnimation {
        id: xAnim
        target: stackView.currentItem
        property: 'x'
        duration: Kirigami.Units.veryLongDuration
        easing.type: Easing.OutCubic
    }

    pushEnter: Transition {
        NumberAnimation {
            property: "x"
            from: (stackView.mirrored ? -0.5 : 0.5) * stackView.width
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutCubic
        }
        NumberAnimation {
            property: "opacity"
            from: 0.0; to: 1.0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutCubic
        }
    }
    pushExit: Transition {
        // Ensure that page can be seen
        NumberAnimation {
            property: "opacity"
            from: 1.0; to: 1.0
        }
    }
    popEnter: Transition {}
    popExit: Transition {
        NumberAnimation {
            property: "x"
            to: (stackView.mirrored ? -1 : 1) * stackView.width
            duration: Kirigami.Units.veryLongDuration
            easing.type: Easing.OutCubic
        }
        NumberAnimation {
            property: "opacity"
            from: 1.0; to: 0.0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutCubic
        }
    }
    replaceEnter: Transition {
        NumberAnimation {
            property: "x"
            from: (stackView.mirrored ? -0.5 : 0.5) * stackView.width
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutCubic
        }
        NumberAnimation {
            property: "opacity"
            from: 0.0; to: 1.0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutCubic
        }
    }
    replaceExit: Transition {
        NumberAnimation {
            property: "x"
            from: 0
            to: (stackView.mirrored ? -0.5 : 0.5) * -stackView.width
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutCubic
        }
        NumberAnimation {
            property: "opacity"
            from: 1.0; to: 0.0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutCubic
        }
    }
}