/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KCLOCK_ALARMPLAYER_H
#define KCLOCK_ALARMPLAYER_H

#include <QMediaPlayer>
#include <QObject>

class AlarmPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
public:
    static AlarmPlayer &instance();
    int volume()
    {
        return m_player->volume();
    };
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
private slots:
    void loopAudio(QMediaPlayer::State state);
};

#endif // KCLOCK_ALARMPLAYER_H
