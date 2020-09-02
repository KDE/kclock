#pragma once

#include <QObject>

class QDBusInterface;
class AlarmWaitWorker;
class Utilities : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kclock.Utilities")
public:
    static Utilities &instance()
    {
        static Utilities singleton;
        return singleton;
    }

    const bool &hasPowerDevil()
    {
        return m_hasPowerDevil;
    };

    int regiser(qint64 timestamp);
    void unregiser(int cookie);
signals:
    void wakeup(int cookie);
public slots:
    void wakeupCallback(int cookie);

private:
    void schedule(); // For AlarmWaitWorker use
    explicit Utilities(QObject *parent = nullptr);

    QDBusInterface *m_interface = nullptr;

    bool m_hasPowerDevil = false;
    QList<int> m_cookies; // token for PowerDevil: https://invent.kde.org/plasma/powerdevil/-/merge_requests/13

    QList<std::tuple<int, long long>> m_list; // cookie, timestamp. For AlarmWaitWorker use
    int m_cookie = 1;                         // For AlarmWaitWorker use
    int m_currentCookie;                      // For AlarmWaitWorker use

    QThread *m_timerThread = nullptr;
    AlarmWaitWorker *m_worker = nullptr;
};
