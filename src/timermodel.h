/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
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

#ifndef KIRIGAMICLOCK_TIMERMODEL_H
#define KIRIGAMICLOCK_TIMERMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <QTimer>

class TimerModel;

class Timer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int length READ length WRITE setLength NOTIFY propertyChanged)
    Q_PROPERTY(int elapsed READ elapsed WRITE setElapsed NOTIFY propertyChanged)
    Q_PROPERTY(QString lengthPretty READ lengthPretty NOTIFY propertyChanged)
    Q_PROPERTY(QString elapsedPretty READ elapsedPretty NOTIFY propertyChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY propertyChanged)
    Q_PROPERTY(bool running READ running NOTIFY propertyChanged)
    Q_PROPERTY(bool finished READ finished NOTIFY propertyChanged)
    Q_PROPERTY(bool justCreated READ justCreated WRITE setJustCreated NOTIFY propertyChanged)

public:
    explicit Timer(QObject *parent = nullptr, int length = 0, int elapsed = 0, QString label = "", bool running = false);
    explicit Timer(const QJsonObject &obj);

    QJsonObject serialize();

    void updateTimer(qint64 duration);
    Q_INVOKABLE void toggleRunning();
    Q_INVOKABLE void reset();

    QString lengthPretty() const
    {
        qint64 len = m_length / 1000, hours = len / 60 / 60, minutes = len / 60 - hours * 60, seconds = len - hours * 60 * 60 - minutes * 60;
        return QString::number(hours) + ":" + QString::number(minutes).rightJustified(2, '0') + ":" + QString::number(seconds).rightJustified(2, '0');
    }
    QString elapsedPretty() const
    {
        qint64 len = m_elapsed / 1000, hours = len / 60 / 60, minutes = len / 60 - hours * 60, seconds = len - hours * 60 * 60 - minutes * 60;
        return QString::number(hours) + ":" + QString::number(minutes).rightJustified(2, '0') + ":" + QString::number(seconds).rightJustified(2, '0');
    }
    int length() const
    {
        return m_length;
    }
    void setLength(int length)
    {
        m_length = length;
        emit propertyChanged();
    }
    int elapsed() const
    {
        return m_elapsed;
    }
    void setElapsed(int elapsed)
    {
        m_elapsed = elapsed;
        emit propertyChanged();
    }
    QString label() const
    {
        return m_label;
    }
    void setLabel(QString label)
    {
        m_label = label;
        emit propertyChanged();
    }
    bool running() const
    {
        return m_running;
    }
    void setRunning(bool running)
    {
        m_running = running;
        emit propertyChanged();
    }
    bool finished() const
    {
        return m_finished;
    }
    void setJustCreated(bool justCreated)
    {
        m_justCreated = justCreated;
        emit propertyChanged();
    }
    bool justCreated() const
    {
        return m_justCreated;
    }

signals:
    void propertyChanged();

private:
    int m_length, m_elapsed; // milliseconds
    QString m_label;
    bool m_running, m_finished, m_justCreated;
};

static TimerModel *timerInst_;

class TimerModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit TimerModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    static void init()
    {
        timerInst_ = new TimerModel();
    }
    static TimerModel *inst()
    {
        return timerInst_;
    }

    void updateTimerLoop();
    void updateTimerStatus(); // update qtimer to be on and off depending on if it is needed (for performance)
    bool areTimersInactive(); // if all timers are inactive

    void load();
    void save();
    Q_INVOKABLE void addNew();
    void insert(int index, Timer *timer);
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void move(int oldIndex, int newIndex);
    Q_INVOKABLE int count();
    Q_INVOKABLE Timer *get(int index);

    bool saveRequested = false;

private:
    QList<Timer *> timerList;
    QTimer *timer;
    QTimer *saveTimer;
};

#endif // KIRIGAMICLOCK_TIMERMODEL_H
