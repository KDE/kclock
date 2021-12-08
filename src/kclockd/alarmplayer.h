/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QMediaPlayer>
#include <QObject>

class AlarmPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)

public:
    static AlarmPlayer &instance();

    int volume();
    Q_INVOKABLE void setVolume(int volume);
    Q_INVOKABLE void setSource(QUrl path);
    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();

Q_SIGNALS:
    void volumeChanged();

protected:
    explicit AlarmPlayer(QObject *parent = nullptr);

private:
    QMediaPlayer *m_player;
    quint64 startPlayingTime = 0;

    bool userStop = false; // indicate if user asks to stop

private Q_SLOTS:
    void loopAudio(QMediaPlayer::State state);
};
