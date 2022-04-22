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
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
, m_player(new QMediaPlayer(this)), m_audio(new QAudioOutput)
#else
, m_player(new QMediaPlayer(this, QMediaPlayer::LowLatency))
#endif
{
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    m_player->setAudioOutput(m_audio);
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &AlarmPlayer::loopAudio);
#else
    connect(m_player, &QMediaPlayer::stateChanged, this, &AlarmPlayer::loopAudio);
#endif
}

#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
void AlarmPlayer::loopAudio(QMediaPlayer::PlaybackState state)
#else
void AlarmPlayer::loopAudio(QMediaPlayer::State state)
#endif
{
    if (!userStop
        && state == QMediaPlayer::StoppedState /* && static_cast<int>(QDateTime::currentSecsSinceEpoch() - startPlayingTime) < settings.alarmSilenceAfter()*/) {
        m_player->play();
    }
}

void AlarmPlayer::play()
{
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
#else
    if (m_player->state() == QMediaPlayer::PlayingState) {
#endif
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
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    return m_audio->volume() * 100;
#else
    return m_player->volume();
#endif
}

void AlarmPlayer::setVolume(int volume)
{
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    m_audio->setVolume(static_cast<float>(volume) / 100);
#else
    m_player->setVolume(volume);
#endif
    Q_EMIT volumeChanged();
}

void AlarmPlayer::setSource(QUrl path)
{
    // if user set a invalid audio path or doesn't even specified a path, resort to default
    if (!path.isValid() || !QFile::exists(path.toLocalFile())) {
        const QUrl url = QUrl::fromLocalFile(
            QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("sounds/freedesktop/stereo/alarm-clock-elapsed.oga")));
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
        m_player->setSource(url);
#else
        m_player->setMedia(url);
#endif
    } else {
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
        m_player->setSource(path);
#else
        m_player->setMedia(path);
#endif
    }
}
