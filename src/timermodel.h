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

#include <QObject>
#include <QString>
#include <QAbstractListModel>

class Timer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 length READ length WRITE setLength NOTIFY propertyChanged)
    Q_PROPERTY(qint64 elapsed READ elapsed WRITE setElapsed NOTIFY propertyChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY propertyChanged)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY propertyChanged)
    
public:
    explicit Timer(QObject *parent = nullptr, qint64 length = 0, qint64 elapsed = 0, QString label = "", bool running = false);
    explicit Timer(QString json);
    
    QString serialize();
    
    void updateTimer(qint64 duration);
    Q_INVOKABLE void toggleRunning();
    Q_INVOKABLE void reset();
    
    qint64 length() const
    {
        return length_;
    }
    void setLength(qint64 length)
    {
        length_ = length;
    }
    qint64 elapsed() const
    {
        return elapsed_;
    }
    void setElapsed(qint64 elapsed)
    {
        elapsed_ = elapsed;
    }
    QString label() const
    {
        return label_;
    }
    void setLabel(QString label)
    {
        label_ = label;
    }
    bool running() const
    {
        return running_;
    }
    void setRunning(bool running)
    {
        running_ = running;
    }
    
signals:
    void propertyChanged();

private:
    qint64 length_, elapsed_; // seconds
    QString label_;
    bool running_;
};

class TimerModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit TimerModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    
    Q_INVOKABLE void timerFinished();
    
    void updateTimerLoop();
    
    void load();
    void save();
    Q_INVOKABLE void addNew();
    void insert(int index, Timer *timer);
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void move(int oldIndex, int newIndex);
    Q_INVOKABLE int count();
    Q_INVOKABLE Timer *get(int index);
    
private:
    QList<Timer *> timerList;
};

#endif // KIRIGAMICLOCK_TIMERMODEL_H
