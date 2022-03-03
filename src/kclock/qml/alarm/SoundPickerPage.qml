/*
 * SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.3
import QtMultimedia 5.15

import org.kde.kirigami 2.12 as Kirigami
import org.kde.kirigamiaddons.sounds 0.1 as Sounds

import "../components"
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
            ListDelegate {
                Layout.fillWidth: true
                Layout.preferredHeight: defaultItem.height
                leftPadding: Kirigami.Units.largeSpacing * 2
                topPadding: root.delegateVerticalPadding
                bottomPadding: root.delegateVerticalPadding
                showSeparator: true
                 
                onClicked: fileDialog.open()
                
                contentItem: RowLayout {
                    width: listView.width
                    Label {
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        text: i18n("Select from files…")
                    }
                }
            }
            
            // default sound
            ListDelegate {
                id: defaultItem
                Layout.fillWidth: true
                leftPadding: Kirigami.Units.largeSpacing * 2
                topPadding: root.delegateVerticalPadding
                bottomPadding: root.delegateVerticalPadding
                showSeparator: listView.count > 0
                
                property string defaultPath: UtilModel.getDefaultAlarmFileLocation()
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
                
                contentItem: RowLayout {
                    width: listView.width
                    
                    Label {
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        text: i18n("Default")
                    }
                    
                    RadioButton {
                        id: radioButton
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        
                        checked: replacePrefix(root.alarmForm.formAudioPath) == replacePrefix(defaultItem.defaultPath)
                        onCheckedChanged: {
                            if (checked) {
                                root.alarmForm.formAudioPath = playablePath(replacePrefix(defaultItem.defaultPath));
                            }
                        }
                    }
                }
            }
        }

        // theme sounds
        delegate: ListDelegate {
            property string sourceUrl: model.sourceUrl
            
            showSeparator: index < listView.count - 1
            
            width: listView.width
            leftPadding: Kirigami.Units.largeSpacing * 2
            topPadding: root.delegateVerticalPadding
            bottomPadding: root.delegateVerticalPadding
            
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
            
            contentItem: RowLayout {
                Label {
                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    text: model.ringtoneName
                }
                
                RadioButton {
                    id: radioButton
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    
                    checked: replacePrefix(root.alarmForm.formAudioPath) == replacePrefix(sourceUrl)
                    onCheckedChanged: {
                        if (checked) {
                            root.alarmForm.formAudioPath = playablePath(replacePrefix(sourceUrl));
                        }
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
