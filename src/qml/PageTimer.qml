import QtQuick 2.0
import QtQuick.Controls 2.0
import org.kde.kirigami 2.0 as Kirigami

Kirigami.Page {
    
    title: "Timer"
    
    Label {
        text: "Time"
    }
    
    mainAction: Kirigami.Action {
        iconName: "edit"
        onTriggered: {}
    }
    rightAction: Kirigami.Action {
        onTriggered: {}
    }
}
