/*
 * Copyright 2020   Han Young <hanyoung@protonmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef ALARMPLAYER_H
#define ALARMPLAYER_H

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

#endif // ALARMPLAYER_H
