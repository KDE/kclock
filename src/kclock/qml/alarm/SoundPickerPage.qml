/*
 * SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 * SPDX-FileCopyrightText: 2023 Nate Graham <nate@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.3
import QtMultimedia 5.15

import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigamiaddons.sounds 0.1 as Sounds

import kclock 1.0

Kirigami.ScrollablePage {
    id: root
    property var alarmForm
    
    title: i18n("Select Alarm Sound")
    
    readonly property int delegateVerticalPadding: Kirigami.Settings.isMobile ? (Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing) : Kirigami.Units.largeSpacing
    
    function playSound() {
        audioPlayer.play();
    }
    
    // HACK: we have issues with file:file:file: being prepended to audio paths
    function replacePrefix(str) {
        return str.replace('file://', '');
    }
    
    function playablePath(str) {
        return str.includes('file://') ? str : ('file://' + str);
    }
    
    ListView {
        id: listView
        model: Sounds.SoundsModel {
            id: soundsModel
            notification: false
        }
        
        Audio {
            id: audioPlayer
            source: root.playablePath(root.alarmForm.formAudioPath);
        }

        header: ColumnLayout {
            width: listView.width
            spacing: 0
            
            // choose from file
            Kirigami.BasicListItem {
                Layout.fillWidth: true
                reserveSpaceForSubtitle: true
                label: i18n("Select from files…")

                onClicked: fileDialog.open()
            }

            // default sound
            Kirigami.BasicListItem {
                id: defaultItem
                property string defaultPath: UtilModel.getDefaultAlarmFileLocation()
                Layout.fillWidth: true
                reserveSpaceForSubtitle: true
                label: i18n("Default")

                onClicked: root.alarmForm.formAudioPath = playablePath(replacePrefix(defaultPath));

                Connections {
                    target: root.alarmForm

                    function onFormAudioPathChanged() {
                        radioButton.checked = replacePrefix(root.alarmForm.formAudioPath) == replacePrefix(defaultItem.defaultPath);
                        if (radioButton.checked) {
                            root.playSound();
                        }
                    }
                }

                trailing: RadioButton {
                    id: radioButton

                    checked: replacePrefix(root.alarmForm.formAudioPath) == replacePrefix(defaultItem.defaultPath)
                    onCheckedChanged: {
                        if (checked) {
                            root.alarmForm.formAudioPath = playablePath(replacePrefix(defaultItem.defaultPath));
                        }
                    }
                }
            }
        }

        // theme sounds
        delegate: Kirigami.BasicListItem {
            property string sourceUrl: model.sourceUrl
            width: listView.width
            reserveSpaceForSubtitle: true
            label: model.ringtoneName

            onClicked: root.alarmForm.formAudioPath = playablePath(replacePrefix(sourceUrl));
            
            Connections {
                target: root.alarmForm
                function onFormAudioPathChanged() {
                    radioButton.checked = replacePrefix(root.alarmForm.formAudioPath) == replacePrefix(sourceUrl);
                    
                    if (radioButton.checked) {
                        root.playSound();
                    }
                }
            }

            trailing: RadioButton {
                id: radioButton

                checked: replacePrefix(root.alarmForm.formAudioPath) == replacePrefix(sourceUrl)
                onCheckedChanged: {
                    if (checked) {
                        root.alarmForm.formAudioPath = playablePath(replacePrefix(sourceUrl));
                    }
                }
            }
        }
        
        // select file dialog
        FileDialog {
            id: fileDialog
            title: i18n("Choose an audio")
            folder: shortcuts.music
            onAccepted: {
                root.alarmForm.formAudioPath = playablePath(replacePrefix(fileDialog.fileUrl.toString()));
                root.playSound();
                close();
            }
            onRejected: close();
            nameFilters: [ i18n("Audio files (*.wav *.mp3 *.ogg *.aac *.flac *.webm *.mka *.opus)"), i18n("All files (*)") ]
        }
    }
}
