import QtQuick 2.4
import QtQuick.Controls 2.1
import org.kde.kirigami 2.2 as Kirigami

Kirigami.Page {
    ListView {
        height: 300
        model: ["Timezone 1", "Timezone 2", "Timezone 3"]
        
        delegate: Component {
            CheckBox {
                text: modelData
            }
        }
    }
}
