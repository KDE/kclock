import QtQuick 2.12
import QtQuick.Controls 2.5
import org.kde.kirigami 2.4 as Kirigami
import QtQuick.Layouts 1.11

/**
 * A large time picker
 * Represented as a clock provides a very visual way for a user
 * to set and visulise a time being chosen
 */

Item {
    id: root

    property int hours
    property int minutes
    property bool pm
    FontMetrics {
        id: fontMetrics
    }
    Component {
        id: hourDelegate
        Label {
            text: modelData + 1
            opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: fontMetrics.font.pixelSize * 1.25
        }
    }
    Component {
        id: minuteDelegate
        Label {
            text: modelData < 10 ? '0' + modelData : modelData
            opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: fontMetrics.font.pixelSize * 1.25
        }
    }
    implicitHeight: 100
    implicitWidth: 200
    RowLayout {
        id: row
        anchors.fill: parent
        Frame {
            background: Rectangle {
                color: "transparent"
                border.color: Kirigami.Theme.highlightColor
                radius: 10
            }
            RowLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Tumbler {
                    id: hoursTumbler
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 3
                    model: 12
                    delegate: hourDelegate
                    visibleItemCount: 3
                    onCurrentIndexChanged: hours = currentIndex + 1
                }
                Label {
                    Layout.alignment: Qt.AlignCenter
                    text: ":"
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 2
                }

                Tumbler {
                    id: minutesTumbler
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 3
                    model: 60
                    delegate: minuteDelegate
                    visibleItemCount: 3
                    currentIndex: minutes
                }
            }
        }
        Switch {
            text: "PM"
            height: parent.height
            onCheckedChanged: {
                if (checked) {
                    text = "AM"
                    pm = false
                }
                else {
                    text = "PM"
                    pm = true
                }
            }
        }
    }
    Component.onCompleted: hoursTumbler.currentIndex = hours
}
