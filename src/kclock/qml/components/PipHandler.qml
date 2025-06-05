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

    signal restoreClicked(pipItem : Item)

    readonly property alias currentItem: stack.currentItem
    readonly property alias supported: window.supported

    readonly property int radius: 15

    // FIXME sometimes this gets stuck when moving the mouse outside the window quickly.
    readonly property bool hovered: hoverHandler.hovered

    readonly property size defaultWindowSize: Qt.size(Kirigami.Units.gridUnit * 15, Kirigami.Units.gridUnit * 15)

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
        // TODO cannot programmatically change the size properly
        // after user has resize it.
        width: pipHandler.defaultWindowSize.width
        height: pipHandler.defaultWindowSize.height
        // TODO minimumWidth/minimumHeight doesn't work.
        visible: !stack.empty

        // When user closes the window, can this happen?
        onVisibleChanged: {
            if (!visible) {
                stack.clear();
            }
        }

        Kirigami.ShadowedRectangle {
            id: shadowRect
            anchors.fill: parent
            anchors.margins: 10
            shadow.size: 10
            radius: pipHandler.radius
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false
            color: Kirigami.Theme.backgroundColor
            border.width: 1
            // Like Kirigami.Separator
            border.color: Kirigami.ColorUtils.linearInterpolation(
                Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor,
                Kirigami.Theme.frameContrast
            )

            QQC2.StackView {
                id: stack
                anchors.fill: parent
                // Don't bleed into the shadow.
                clip: true
            }

            HoverHandler {
                id: hoverHandler
            }

            Item {
                anchors {
                    fill: parent
                    margins: Kirigami.Units.largeSpacing
                }
                opacity: hoverHandler.hovered ? 1 : 0
                visible: opacity > 0

                Behavior on opacity {
                    NumberAnimation {
                        duration: Kirigami.Units.longDuration
                        easing.type: Easing.InOutQuad
                    }
                }

                QQC2.ToolButton {
                    anchors {
                        top: parent.top
                        left: parent.left
                    }
                    icon.name: "window-restore-pip"
                    text: i18nc("@action:button Close pip and return to the page where the content was popped out from", "Back to app")
                    display: QQC2.AbstractButton.IconOnly
                    onClicked: {
                        pipHandler.restoreClicked(stack.currentItem);
                        pipHandler.hide();
                    }

                    QQC2.ToolTip.text: text
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    QQC2.ToolTip.visible: Kirigami.Settings.tabletMode ? pressed : hovered
                }

                QQC2.ToolButton {
                    anchors {
                        top: parent.top
                        right: parent.right
                    }
                    icon.name: "window-close"
                    text: i18nc("@action:button", "Close")
                    display: QQC2.AbstractButton.IconOnly
                    onClicked: {
                        pipHandler.hide();
                    }

                    QQC2.ToolTip.text: text
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                    QQC2.ToolTip.visible: Kirigami.Settings.tabletMode ? pressed : hovered
                }
            }
        }
    }
}
