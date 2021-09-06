/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.12 as Kirigami
import QtGraphicalEffects 1.12

ToolBar {
    id: toolbarRoot
    padding: 0
    
    background: Rectangle {
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false
        color: Kirigami.Theme.backgroundColor
        anchors.fill: parent
        
        layer.enabled: true
        layer.effect: RectangularGlow {
            glowRadius: 5
            spread: 0.3
            color: Qt.rgba(0.0, 0.0, 0.0, 0.15)
        }
    }
    
    RowLayout {
        anchors.fill: parent
        spacing: 0
        Repeater {
            model: ListModel {
                // we can't use i18n with ListElement
                Component.onCompleted: {
                    append({"name": i18n("Time"), "icon": "clock"});
                    append({"name": i18n("Timers"), "icon": "player-time"});
                    append({"name": i18n("Stopwatch"), "icon": "chronometer"});
                    append({"name": i18n("Alarms"), "icon": "notifications"});
                    append({"name": i18n("Settings"), "icon": "settings-configure"});
                }
            }
            
            Rectangle {
                Layout.minimumWidth: parent.width / 5
                Layout.maximumWidth: parent.width / 5
                Layout.preferredHeight: Kirigami.Units.gridUnit * 3 + Kirigami.Units.smallSpacing * 2
                Layout.alignment: Qt.AlignCenter
                
                Kirigami.Theme.colorSet: Kirigami.Theme.Window
                Kirigami.Theme.inherit: false
                
                color: mouseArea.pressed ? Qt.darker(Kirigami.Theme.backgroundColor, 1.1) : 
                       mouseArea.containsMouse ? Qt.darker(Kirigami.Theme.backgroundColor, 1.03) : Kirigami.Theme.backgroundColor
                
                property bool isCurrentPage: appwindow.getPage(model.name) === appwindow.pageStack.currentItem
                
                Behavior on color { ColorAnimation { duration: Kirigami.Units.shortDuration } }
                
                // top highlight rectangle (if delegate is selected)
                Rectangle {
                    id: highlightRectangle
                    opacity: isCurrentPage ? 1 : 0
                    color: Kirigami.Theme.highlightColor
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 2
                    
                    Behavior on opacity { NumberAnimation { duration: Kirigami.Units.longDuration } }
                }
                Rectangle {
                    id: highlightShadow
                    opacity: isCurrentPage ? 0.3 : 0
                    anchors.top: highlightRectangle.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 2
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: highlightRectangle.color }
                        GradientStop { position: 1.0; color: "transparent" }
                    }
                    
                    Behavior on opacity { NumberAnimation { duration: Kirigami.Units.longDuration } }
                }
                
                // mouse/touch event
                MouseArea {
                    id: mouseArea
                    hoverEnabled: true
                    anchors.fill: parent
                    onClicked: {
                        if (!isCurrentPage) {
                            appwindow.switchToPage(appwindow.getPage(model.name), 0);
                        }
                    }
                }
                
                // delegate content
                ColumnLayout {
                    id: itemColumn
                    anchors.fill: parent
                    anchors.topMargin: Kirigami.Units.smallSpacing
                    anchors.bottomMargin: Kirigami.Units.smallSpacing
                    spacing: Kirigami.Units.smallSpacing
                    
                    Kirigami.Icon {
                        source: model.icon
                        isMask: true
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                        Layout.preferredHeight: Math.round(Kirigami.Units.iconSizes.small * 1.5)
                        Layout.preferredWidth: Math.round(Kirigami.Units.iconSizes.small * 1.5)
                        
                        opacity: isCurrentPage ? 1 : 0.7
                        color: isCurrentPage ? Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.highlightColor, Kirigami.Theme.textColor, 0.5) : Kirigami.Theme.textColor
                        
                        Behavior on color { ColorAnimation {} }
                        Behavior on opacity { NumberAnimation {} }
                    }
                    
                    Label {
                        text: i18n(model.name)
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideMiddle
                        
                        opacity: isCurrentPage ? 1 : 0.7
                        color: isCurrentPage ? Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.highlightColor, Kirigami.Theme.textColor, 0.5) : Kirigami.Theme.textColor
                        font.bold: isCurrentPage
                        font.family: Kirigami.Theme.smallFont.family
                        font.pointSize: Kirigami.Theme.smallFont.pointSize

                        Behavior on color { ColorAnimation {} }
                        Behavior on opacity { NumberAnimation {} }
                    }
                }
            }
        }
    }
} 
