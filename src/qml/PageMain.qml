import QtQuick 2.0
import QtQuick.Controls 2.3
import org.kde.kirigami 2.0 as Kirigami

Kirigami.Page {
    
    title: "Time"
    
    Label {
        id: timeText
        anchors.horizontalCenter: parent.horizontalCenter
        color: Kirigami.Theme.highlightColor
        font.pointSize: 40
        text: Qt.formatTime(new Date(), "hh:mm:ss")
    }
    
    Timer {
        id: timer
        interval: 1000
        repeat: true
        running: true
        onTriggered: {
            timeText.text = Qt.formatTime(new Date(), "hh:mm:ss")
        }
    }
    
    PageTimezoneSelect{
        id: pagetimezone
    }
    
    mainAction: Kirigami.Action {
        iconName: "entry-edit"
        onTriggered: {
            pageStack.push(pagetimezone)
        }
    }
}
