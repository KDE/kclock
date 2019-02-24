import QtQuick 2.0
import QtQuick.Controls 2.0
import org.kde.kirigami 2.0 as Kirigami

Kirigami.Page {
    id: stopwatchpage
    title: "Stopwatch"
    property bool running: false
    property int elapsedTime: 0
    property double displayTime: elapsedTime

    Label {
        id: timeLabel
        text: (displayTime / 1000).toFixed(3)
        anchors.horizontalCenter: parent.horizontalCenter
        color: Kirigami.Theme.highlightColor
        font.pointSize: 40
    }

    ListView {
        anchors.top: timeLabel.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        model: roundModel
        delegate: Kirigami.BasicListItem {
            label: "Round #" + (model.index + 1) + ": " + model.time / 1000
        }
    }

    Timer {
        id: stopwatchTimer
        interval: 16
        running: stopwatchpage.running
        repeat: true
        onTriggered: {
            elapsedTime += interval
        }
    }

    ListModel {
        id: roundModel
    }
    
    mainAction: Kirigami.Action {
        iconName: running ? "media-playback-pause" : "media-playback-start"
        tooltip: running ? "Pause" : "Start"
        onTriggered: {
            running = !running
        }
    }
    
    rightAction: Kirigami.Action {
        iconName: "contact-new"
        tooltip: "New round"
        onTriggered: {
            roundModel.append({ time: elapsedTime })
        }
    }
    
    leftAction: Kirigami.Action {
        iconName: "media-playback-stop"
        tooltip: "Reset"
        onTriggered: {
            running = false;
            elapsedTime = 0
            roundModel.clear()
        }
    }
}
