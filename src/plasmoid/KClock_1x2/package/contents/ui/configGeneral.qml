/*
    SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import org.kde.kirigami 2.12 as Kirigami

Kirigami.FormLayout {
    id: page
  
    property alias cfg_twelveHourTime: twelveHourTime.checked
    property alias cfg_showDate: showDate.checked
    property alias cfg_showAlarms: showAlarms.checked
    property string cfg_textAlignment

    CheckBox {
        id: twelveHourTime
        text: i18n("Use 12 hour time")
    }
    CheckBox {
        id: showDate
        text: i18n("Show date")
    }
    CheckBox {
        id: showAlarms
        text: i18n("Show alarms")
    }
    ComboBox {
        id: textAlignment
        Kirigami.FormData.label: i18n("Text alignment:")
        textRole: 'label'
        model: [
            {
                'label': i18n("Left"),
                'name': "Left",
            },
            {
                'label': i18n("Center"),
                'name': "Center",
            },
            {
                'label': i18n("Right"),
                'name': "Right",
            }
        ]
        onCurrentIndexChanged: cfg_textAlignment = model[currentIndex]["name"]
        Component.onCompleted: {
            if (plasmoid.configuration.textAlignment == "") {
                plasmoid.configuration.textAlignment = "Center";
            }
            for (var i = 0; i < model.length; i++) {
                if (model[i]["name"] === plasmoid.configuration.textAlignment) {
                    textAlignment.currentIndex = i;
                }
            }
        }
    }
}
