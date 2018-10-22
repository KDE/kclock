import QtQuick 2.0
import QtQuick.Controls 2.0
import org.kde.kirigami 2.2 as Kirigami

Kirigami.Page {
    
    title: "Timer"
    id: timerpage
    property bool running: false
    property int timerDuration: 60
    property int elapsedTime: 0
    
    Label {
        text: (timerDuration - elapsedTime / 1000).toFixed(1)
        color: Kirigami.Theme.highlightColor
        font.pointSize: 40
    }

    Timer {
        interval: 100
        running: timerpage.running
        repeat: true
        onTriggered: {
            elapsedTime += interval
        }
    }
    
    mainAction: Kirigami.Action {
        iconName: running ? "media-playback-pause" : "media-playback-start"
        onTriggered: {
            running = !running
        }
    }

    leftAction: Kirigami.Action {
        iconName: "media-playback-stop"
        onTriggered: {
            elapsedTime = 0
            running = false
        }
    }

    Kirigami.OverlaySheet {
        id: timerEditSheet

        Column {
            Text {
                text: "Change timer duration"
            }
            Row {
                SpinBox {
                    id: spinBoxMinutes
                    onValueChanged: timerEditSheet.setDuration()
                    value: timerDuration / 60
                }

                SpinBox {
                    id: spinBoxSeconds
                    to: 60
                    onValueChanged: timerEditSheet.setDuration()
                    value: timerDuration % 60
                }
            }
        }

        function setDuration() {
            timerDuration = spinBoxMinutes.value * 60 + spinBoxSeconds.value
        }
    }

    rightAction: Kirigami.Action {
        onTriggered: {
            timerEditSheet.open()
        }
    }
}
