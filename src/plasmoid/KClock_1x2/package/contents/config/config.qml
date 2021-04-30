/*
    SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

import QtQuick 2.0
import QtQml 2.2

ConfigModel {
    id: configModel

    ConfigCategory {
         name: i18n("General")
         icon: "configure"
         source: "configGeneral.qml"
    }
}
