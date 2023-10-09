/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "alarmplayer.h"

#include <QDateTime>
#include <QFile>
#include <QStandardPaths>

AlarmPlayer &AlarmPlayer::instance()
{
    static AlarmPlayer singleton;
    return singleton;
}

AlarmPlayer::AlarmPlayer(QObject *parent)
    : QObject{parent}
    , m_player(new QMediaPlayer(this))
    , m_audio(new QAudioOutput)
{
    m_player->setAudioOutput(m_audio);
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &AlarmPlayer::loopAudio);
}

void AlarmPlayer::loopAudio(QMediaPlayer::PlaybackState state)
{
    if (!userStop
        && state == QMediaPlayer::StoppedState /* && static_cast<int>(QDateTime::currentSecsSinceEpoch() - startPlayingTime) < settings.alarmSilenceAfter()*/) {
        m_player->play();
    }
}

void AlarmPlayer::play()
{
    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
        return;
    }

    startPlayingTime = QDateTime::currentSecsSinceEpoch();
    userStop = false;
    m_player->play();
}

void AlarmPlayer::stop()
{
    userStop = true;
    m_player->stop();
}

int AlarmPlayer::volume()
{
    return m_audio->volume() * 100;
}

void AlarmPlayer::setVolume(int volume)
{
    m_audio->setVolume(static_cast<float>(volume) / 100);
    Q_EMIT volumeChanged();
}

void AlarmPlayer::setSource(const QUrl &path)
{
    // if user set a invalid audio path or doesn't even specified a path, resort to default
    if (!path.isValid() || !QFile::exists(path.toLocalFile())) {
        const QUrl url = QUrl::fromLocalFile(
            QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("sounds/freedesktop/stereo/alarm-clock-elapsed.oga")));
        m_player->setSource(url);
    } else {
        m_player->setSource(path);
    }
}
