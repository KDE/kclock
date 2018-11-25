import QtQuick 2.0
import QtQuick.Controls 2.3
import org.kde.kirigami 2.4 as Kirigami

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

    Kirigami.CardsListView {
        model: timeZoneShowModel
        anchors.top: timeText.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        clip: true
        delegate: Kirigami.Card {
            Label {
                text: model.name
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
            }
            Label {
                text: model.timeString
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
            }
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
