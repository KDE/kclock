#include "kclockformat.h"
#include <QDebug>
#include <QLocale>
#include <QTime>
#include <QTimer>
KclockFormat::KclockFormat(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &KclockFormat::updateTime);
    m_timer->setSingleShot(true);

    // time to next minute
    m_timer->start((60 - (QTime::currentTime().msecsSinceStartOfDay() / 1000) % 60) * 1000); // seconds to next minute
}

void KclockFormat::updateTime()
{
    m_currentTime = QLocale::system().toString(QTime::currentTime(), QLocale::ShortFormat);
    Q_EMIT timeChanged();
    disconnect(m_timer, &QTimer::timeout, this, &KclockFormat::updateTime);
    m_timer->setSingleShot(false);
    connect(m_timer, &QTimer::timeout, [this] {
        this->m_currentTime = QLocale::system().toString(QTime::currentTime(), QLocale::ShortFormat);
        Q_EMIT this->timeChanged();
    });
    m_timer->start(60 * 1000);
}

QString KclockFormat::formatTimeString(int hours, int minutes)
{
    return QLocale::system().toString(QTime(hours, minutes), QLocale::ShortFormat);
}

bool KclockFormat::isChecked(int dayIndex, int daysOfWeek)
{
    // convert start day to standard start day of week, Monday
    dayIndex += QLocale::system().firstDayOfWeek() - 1;
    while (dayIndex > 6)
        dayIndex -= 7;

    int day = 1;
    day <<= dayIndex;
    return daysOfWeek & day;
}

WeekModel::WeekModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // init week and flag value
    int dayFlag[] = {1, 2, 4, 8, 16, 32, 64}, i = 0;
    for (int j = QLocale::system().firstDayOfWeek(); j <= 7; j++) {
        m_listItem[i++] = std::tuple<QString, int>(QLocale::system().dayName(j, QLocale::ShortFormat), dayFlag[j - 1]);
    }

    for (int j = 1; j < QLocale::system().firstDayOfWeek(); j++) {
        m_listItem[i++] = std::tuple<QString, int>(QLocale::system().dayName(j, QLocale::ShortFormat), dayFlag[j - 1]);
    }
}

int WeekModel::rowCount(const QModelIndex &parent) const
{
    return 7;
}

QVariant WeekModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= 7) {
        return QVariant();
    }
    if (role == NameRole)
        return std::get<0>(m_listItem[index.row()]);
    else if (role == FlagRole)
        return std::get<1>(m_listItem[index.row()]);
    else
        return QVariant();
}

QHash<int, QByteArray> WeekModel::roleNames() const
{
    return {{NameRole, "name"}, {FlagRole, "flag"}};
}
