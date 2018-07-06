import QtQuick 2.0
import org.kde.kirigami 2.2 as Kirigami

Kirigami.Page {
    
    title: "Alarms"
    
    mainAction: Kirigami.Action {
        onTriggered: pageStack.push(pageeditalarm)
    }
    
    PageEditAlarm {
        id: pageeditalarm
    }
}
