import QtQuick 2.4
import QtQuick.Controls 2.1
import org.kde.kirigami 2.2 as Kirigami

Kirigami.Page {
    
    title: "Edit Alarm"
    
    CheckBox{
        text: "Enabled"
    }
    CheckBox {
        text: "Repeat"
    }
    mainAction: Kirigami.Action {
        //Save
    }
}
