import QtQuick 2.11
import QtQuick.Controls 2.4
import org.kde.kirigami 2.4 as Kirigami

Kirigami.Page {
    
    title: "Alarms"
    
    mainAction: Kirigami.Action {
        onTriggered: alarmModel.addAlarm()
    }
    
    Kirigami.CardsListView {
        model: alarmModel
        anchors.fill: parent
        delegate: Kirigami.Card {
            CheckBox {
                text: model.name
                checked: model.enabled
            }
        }
    }
}
