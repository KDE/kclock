import QtQuick 2.11
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.2 as Kirigami
import app.test 1.0

Kirigami.Page {

    title: "Timezones"

    ColumnLayout {
    
    TextInput {

    }

    ListView {
        model: timeZones
        delegate: Row {
            CheckBox {
                checked: model.shown
            }
            Label {
                text: model.name
            }
        }
    }
    TimeZoneModel {
        id: timeZones
    }}
}
