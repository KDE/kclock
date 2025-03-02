import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Templates as T

import org.kde.kirigami as Kirigami

ToolBar {
    id: root

    property string title

    property list<T.Action> actions

    property bool showBackButton: false

    position: ToolBar.Header
    Layout.fillWidth: true

    RowLayout {
        anchors.fill: parent
        spacing: 0

        ToolButton {
            id: backButton
            visible: showBackButton
            icon.name: (LayoutMirroring.enabled ? "go-previous-symbolic-rtl" : "go-previous-symbolic")
            text: i18n('Navigate Back')
            display: AbstractButton.IconOnly
            onClicked: applicationWindow().pageStack.pop()

            ToolTip {
                visible: backButton.hovered
                text: backButton.text
                delay: Kirigami.Units.toolTipDelay
                timeout: 5000
                y: backButton.height
            }
        }

        Kirigami.Heading {
            Layout.minimumHeight: backButton.height // Keep RowLayout height to be at least button height
            Layout.leftMargin: Kirigami.Units.largeSpacing
            text: root.title
            elide: Label.ElideRight
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignVCenter
            level: 1
        }

        Kirigami.ActionToolBar {
            Layout.fillWidth: true
            Layout.leftMargin: Kirigami.Units.largeSpacing
            alignment: Qt.AlignRight
            Layout.fillHeight: true
            actions: root.actions
        }
    }
}