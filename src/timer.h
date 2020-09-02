#pragma once

#include <QObject>

class Timer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint64 length READ length WRITE setLength NOTIFY propertyChanged)
    Q_PROPERTY(quint64 elapsed READ elapsed WRITE setElapsed NOTIFY propertyChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY propertyChanged)
    Q_PROPERTY(bool running READ running NOTIFY propertyChanged)
    Q_PROPERTY(bool finished READ finished NOTIFY propertyChanged)
    Q_PROPERTY(bool justCreated READ justCreated WRITE setJustCreated NOTIFY propertyChanged)

public:
    explicit Timer(QObject *parent = nullptr, int length = 0, int elapsed = 0, QString label = QStringLiteral(), bool running = false);
    explicit Timer(const QJsonObject &obj);

    QJsonObject serialize();

    void updateTimer(qint64 duration);
    Q_INVOKABLE void toggleRunning();
    Q_INVOKABLE void reset();

    const quint64& length() const
    {
        return m_length;
    }
    void setLength(int length)
    {
        m_length = length;
        Q_EMIT propertyChanged();
    }
    const quint64& elapsed() const
    {
        return m_elapsed;
    }
    void setElapsed(int elapsed)
    {
        m_elapsed = elapsed;
        Q_EMIT propertyChanged();
    }
    const QString& label() const
    {
        return m_label;
    }
    void setLabel(QString label)
    {
        m_label = label;
        Q_EMIT propertyChanged();
    }
    const bool& running() const
    {
        return m_running;
    }
    void setRunning(bool running)
    {
        m_running = running;
        Q_EMIT propertyChanged();
    }
    const bool& finished() const
    {
        return m_finished;
    }
    void setJustCreated(bool justCreated)
    {
        m_justCreated = justCreated;
        Q_EMIT propertyChanged();
    }
    const bool& justCreated() const
    {
        return m_justCreated;
    }

signals:
    void propertyChanged();

private:
    unsigned long long m_length, m_elapsed = 0; // milliseconds
    QString m_label;
    bool m_running, m_finished, m_justCreated;
};
