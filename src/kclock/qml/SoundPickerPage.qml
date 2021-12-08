/*
 * SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2

import org.kde.kirigami 2.12 as Kirigami
import org.kde.kirigamiaddons.sounds 0.1 as Sounds

Kirigami.ScrollablePage {
    
    title: i18n("Select sound")
    
    Sounds.SoundsPicker {
        
    }
}
