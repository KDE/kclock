/*
 * SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 * SPDX-FileCopyrightText: 2023 Nate Graham <nate@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtMultimedia

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.sounds 0.1 as Sounds
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.kclock

Kirigami.ScrollablePage {
    id: root
    property var alarmForm

    title: i18n("Select Alarm Sound")

    readonly property int delegateVerticalPadding: Kirigami.Settings.isMobile ? (Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing) : Kirigami.Units.largeSpacing

    function playSound() : void {
        audioPlayer.play();
    }

    // HACK: we have issues with file: being prepended to audio paths
    function replacePrefix(path: string): string {
        return path.replace('file://', '');
    }

    function playablePath(str) {
        return str.includes('file://') ? str : ('file://' + str);
    }

    ListView {
        id: listView
        model: Sounds.SoundsModel {
            id: soundsModel
            notification: false
            theme: 'freedesktop'
        }
        currentIndex: -1

        MediaPlayer {
            id: audioPlayer
            source: root.playablePath(root.alarmForm.formAudioPath);
            audioOutput: AudioOutput {}
        }

        header: ColumnLayout {
            width: listView.width
            spacing: 0

            // choose from file
            Delegates.RoundedItemDelegate {
                id: selectFromFileItem
                text: i18n("Select from files…")
                onClicked: fileDialog.open()
                Layout.fillWidth: true
            }

            // default sound
            Delegates.RoundedItemDelegate {
                id: defaultItem

                property string defaultPath: root.replacePrefix(UtilModel.getDefaultAlarmFileLocation())

                text: i18n("Default")

                onClicked: root.alarmForm.formAudioPath = playablePath(defaultPath);

                Connections {
                    target: root.alarmForm

                    function onFormAudioPathChanged() {
                        radioButton.checked = replacePrefix(root.alarmForm.formAudioPath) == defaultItem.defaultPath;
                        if (radioButton.checked) {
                            root.playSound();
                        }
                    }
                }

                Layout.fillWidth: true

                contentItem: RowLayout {
                    Delegates.DefaultContentItem {
                        itemDelegate: defaultItem
                    }

                    RadioButton {
                        id: radioButton

                        checked: replacePrefix(root.alarmForm.formAudioPath) === defaultItem.defaultPath
                        onCheckedChanged: {
                            if (checked) {
                                root.alarmForm.formAudioPath = playablePath(defaultItem.defaultPath);
                            }
                        }
                    }
                }
            }
        }

        // theme sounds
        delegate: Delegates.RoundedItemDelegate {
            id: soundDelegate

            required property string ringtoneName
            required property string sourceUrl // instead of url, so we don't have type errors
            required property int index

            text: ringtoneName

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
                Delegates.DefaultContentItem {
                    itemDelegate: soundDelegate
                }

                RadioButton {
                    id: radioButton

                    checked: replacePrefix(root.alarmForm.formAudioPath) == replacePrefix(soundDelegate.sourceUrl)
                    onCheckedChanged: {
                        if (checked) {
                            root.alarmForm.formAudioPath = playablePath(replacePrefix(soundDelegate.sourceUrl));
                        }
                    }
                }
            }
        }

        // select file dialog
        FileDialog {
            id: fileDialog
            title: i18n("Choose an audio")
            currentFolder: StandardPaths.standardLocations(StandardPaths.MusicLocation)[0]
            onAccepted: {
                root.alarmForm.formAudioPath = playablePath(replacePrefix(fileDialog.selectedFile.toString()));
                root.playSound();
                close();
            }
            onRejected: close();
            nameFilters: [ i18n("Audio files (*.wav *.mp3 *.ogg *.aac *.flac *.webm *.mka *.opus)"), i18n("All files (*)") ]
        }
    }
}
