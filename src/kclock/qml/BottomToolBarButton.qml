import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.12 as Kirigami
Rectangle {
    property string buttonName: ""
    property string buttonIcon: ""
    Layout.minimumWidth: parent.width / 5
    Layout.maximumWidth: parent.width / 5
    Layout.preferredHeight: Kirigami.Units.gridUnit * 3
    Layout.alignment: Qt.AlignCenter

    Kirigami.Theme.colorSet: Kirigami.Theme.Window
    Kirigami.Theme.inherit: false

    color: mouseArea.pressed ? Qt.darker(Kirigami.Theme.backgroundColor, 1.1) :
           mouseArea.containsMouse ? Qt.darker(Kirigami.Theme.backgroundColor, 1.03) : Kirigami.Theme.backgroundColor

    property bool isCurrentPage: appwindow.currentPage === buttonName

    Behavior on color {
        ColorAnimation {
            duration: 100
            easing.type: Easing.InOutQuad
        }
    }

    MouseArea {
        id: mouseArea
        hoverEnabled: true
        anchors.fill: parent
        onClicked: {
            if (!isCurrentPage) {
                appwindow.switchToPage(appwindow.getPage(buttonName), 0);
                appwindow.currentPage = buttonName;
            }
        }
        onPressed: {
            widthAnim.to = toolbarRoot.shrinkIconSize;
            heightAnim.to = toolbarRoot.shrinkIconSize;
            fontAnim.to = toolbarRoot.shrinkFontSize;
            widthAnim.restart();
            heightAnim.restart();
            fontAnim.restart();
        }
        onReleased: {
            if (!widthAnim.running) {
                widthAnim.to = toolbarRoot.iconSize;
                widthAnim.restart();
            }
            if (!heightAnim.running) {
                heightAnim.to = toolbarRoot.iconSize;
                heightAnim.restart();
            }
            if (!fontAnim.running) {
                fontAnim.to = toolbarRoot.fontSize;
                fontAnim.restart();
            }
        }
    }

    ColumnLayout {
        id: itemColumn
        anchors.fill: parent
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Icon {
            color: isCurrentPage ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
            source: buttonIcon
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.preferredHeight: toolbarRoot.iconSize
            Layout.preferredWidth: toolbarRoot.iconSize

            ColorAnimation on color {
                easing.type: Easing.Linear
            }
            NumberAnimation on Layout.preferredWidth {
                id: widthAnim
                easing.type: Easing.Linear
                duration: 130
                onFinished: {
                    if (widthAnim.to !== toolbarRoot.iconSize && !mouseArea.pressed) {
                        widthAnim.to = toolbarRoot.iconSize;
                        widthAnim.start();
                    }
                }
            }
            NumberAnimation on Layout.preferredHeight {
                id: heightAnim
                easing.type: Easing.Linear
                duration: 130
                onFinished: {
                    if (heightAnim.to !== toolbarRoot.iconSize && !mouseArea.pressed) {
                        heightAnim.to = toolbarRoot.iconSize;
                        heightAnim.start();
                    }
                }
            }
        }

        Label {
            color: isCurrentPage ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
            text: i18n(buttonName)
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideMiddle
            font.pointSize: toolbarRoot.fontSize

            ColorAnimation on color {
                easing.type: Easing.Linear
            }
            NumberAnimation on font.pointSize {
                id: fontAnim
                easing.type: Easing.Linear
                duration: 130
                onFinished: {
                    if (fontAnim.to !== toolbarRoot.fontSize && !mouseArea.pressed) {
                        fontAnim.to = toolbarRoot.fontSize;
                        fontAnim.start();
                    }
                }
            }
        }
    }
}
