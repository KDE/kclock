import QtQuick 2.11
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.2 as Kirigami
import app.test 1.0

Kirigami.Page {
    
    property TimeZoneModel timeZones

    title: "Timezones"
    
    TextField {
        id: timeZoneSearchInput
        anchors.right: parent.right
        anchors.left: parent.left
        placeholderText: "Search"
        onTextChanged: timeZones.setFilterFixedString(text)
    }

    ListView {
        anchors.top: timeZoneSearchInput.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.topMargin: Kirigami.Units.smallSpacing * 2
        clip: true
        spacing: Kirigami.Units.smallSpacing
        model: timeZones
        delegate: Row {
            CheckBox {
                checked: model.shown
                text: model.name
                onClicked: model.shown = this.checked
            }
        }
    }
}
