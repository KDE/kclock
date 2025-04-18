/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QMediaPlayer>
#include <QObject>

#include <QAudioOutput>

#include <KConfigWatcher>

class AlarmPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)

public:
    static AlarmPlayer &instance();

    int volume();
    Q_INVOKABLE void setVolume(int volume);
    Q_INVOKABLE void setSource(const QString &path);
    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();

    QString soundThemeName() const;
    QString defaultAlarmSoundPath() const;

Q_SIGNALS:
    void volumeChanged();

protected:
    explicit AlarmPlayer(QObject *parent = nullptr);

private:
    QMediaPlayer *m_player;
    QAudioOutput *m_audio;
    KConfigWatcher::Ptr m_soundThemeWatcher;

    bool m_userStop = false; // indicate if user asks to stop

private Q_SLOTS:
    void loopAudio(QMediaPlayer::PlaybackState state);
};
