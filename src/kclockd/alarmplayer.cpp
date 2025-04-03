/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "alarmplayer.h"

#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>

#include <KConfigGroup>
#include <KSharedConfig>

using namespace Qt::Literals::StringLiterals;

AlarmPlayer &AlarmPlayer::instance()
{
    static AlarmPlayer singleton;
    return singleton;
}

AlarmPlayer::AlarmPlayer(QObject *parent)
    : QObject{parent}
    , m_player(new QMediaPlayer(this))
    , m_audio(new QAudioOutput)
    , m_soundThemeWatcher(KConfigWatcher::create(KSharedConfig::openConfig(QStringLiteral("kdeglobals"))))
{
    m_player->setAudioOutput(m_audio);
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &AlarmPlayer::loopAudio);
}

void AlarmPlayer::loopAudio(QMediaPlayer::PlaybackState state)
{
    if (!m_userStop && state == QMediaPlayer::StoppedState) {
        m_player->play();
    }
}

void AlarmPlayer::play()
{
    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
        return;
    }

    m_userStop = false;
    m_player->play();
}

void AlarmPlayer::stop()
{
    m_userStop = true;
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

void AlarmPlayer::setSource(const QString &path)
{
    if (path.isEmpty() || !QFileInfo::exists(path)) {
        m_player->setSource(QUrl::fromLocalFile(defaultAlarmSoundPath()));
    } else {
        m_player->setSource(QUrl::fromLocalFile(path));
    }
}

QString AlarmPlayer::soundThemeName() const
{
    const KConfigGroup soundGroup = m_soundThemeWatcher->config()->group(QStringLiteral("Sounds"));
    const QString themeName = soundGroup.readEntry(QStringLiteral("Theme"), QStringLiteral("ocean"));
    return themeName;
}

QString AlarmPlayer::defaultAlarmSoundPath() const
{
    const QString soundPath = QStringLiteral("sounds/%1/stereo/alarm-clock-elapsed.%2");

    for (const QString &theme : {soundThemeName(), u"freedesktop"_s}) {
        for (const QLatin1String extension : {"wav"_L1, "oga"_L1, "ogg"_L1}) {
            const QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, soundPath.arg(theme, extension));
            if (!path.isEmpty()) {
                return path;
            }
        }
    }

    qCritical() << "Failed to find any alarm clock sound!";
    return QString();
}

#include "moc_alarmplayer.cpp"
