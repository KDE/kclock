#pragma once

#include <QDateTime>
#include <QObject>
class Timer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)

public:
    explicit Timer(int length = 0, QString label = QStringLiteral(), bool running = false);
    explicit Timer(const QJsonObject &obj);

    QJsonObject serialize();

    Q_SCRIPTABLE void toggleRunning();
    Q_SCRIPTABLE void reset();
    Q_SCRIPTABLE int elapsed() const
    {
        if (running())
            return QDateTime::currentSecsSinceEpoch() - m_startTime;
        else
            return m_hasElapsed;
    }
    const int &length() const
    {
        return m_length;
    }
    void setLength(int length)
    {
        m_length = length;
        Q_EMIT lengthChanged();
    }
    const QString &label() const
    {
        return m_label;
    }
    void setLabel(QString label)
    {
        m_label = label;
        Q_EMIT labelChanged();
    }
    const bool &running() const
    {
        return m_running;
    }

signals:
    void lengthChanged();
    void labelChanged();
    void runningChanged();
private slots:
    void timeUp(int cookie);

private:
    void setRunning(bool running);
    void sendNotification();

    int m_length, m_startTime = 0; // seconds
    int m_hasElapsed = 0;          // time has elapsed till stop, only be updated if stopped or finished
    int m_cookie = -1;
    QString m_label;
    bool m_running;
};
