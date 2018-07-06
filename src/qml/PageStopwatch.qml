import QtQuick 2.0
import QtQuick.Controls 2.0
import org.kde.kirigami 2.0 as Kirigami

Kirigami.Page {
    title: "Stopwatch"
    property bool running: false
    
    Label {
        id: timeLabel
        text: "00:00:00"
        anchors.horizontalCenter: parent.horizontalCenter
        color: Kirigami.Theme.highlightColor
        font.pointSize: 40
    }
    
    mainAction: Kirigami.Action {
        iconName: running ? "media-playback-pause" : "media-playback-start"
        tooltip: running ? "Pause" : "Start"
        onTriggered: running = !running
    }
    
    rightAction: Kirigami.Action {
        iconName: "contact-new"
        tooltip: "New round"
    }
    
    leftAction: Kirigami.Action {
        iconName: "media-playback-stop"
        tooltip: "Reset"
        onTriggered: running = false
    }
}
