import QtQuick 2.0
import QtQuick.Controls 2.0
import org.kde.kirigami 2.0 as Kirigami

Kirigami.Page {
    id: stopwatchpage
    title: "Stopwatch"
    property bool running: false
    property var rounds: []
    property int elapsedTime: 0
    property double displayTime: elapsedTime

    Label {
        id: timeLabel
        text: (displayTime / 1000).toFixed(3)
        anchors.horizontalCenter: parent.horizontalCenter
        color: Kirigami.Theme.highlightColor
        font.pointSize: 40
    }

    Timer {
        id: stopwatchTimer
        interval: 16
        running: true
        repeat: true
        onTriggered: {
            if(stopwatchpage.running) {
                elapsedTime += 16;
            }
        }
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
            rounds.push(currentTime);
        }
    }
    
    leftAction: Kirigami.Action {
        iconName: "media-playback-stop"
        tooltip: "Reset"
        onTriggered: {
            running = false;
            elapsedTime = 0
        }
    }
}
