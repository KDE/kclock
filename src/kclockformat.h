#ifndef KCLOCKFORMAT_H
#define KCLOCKFORMAT_H

#include <QObject>
#include <QAbstractListModel>
#include <array>
#include <tuple>
class QTimer;
class WeekModel;
class KclockFormat : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentTime READ currentTime NOTIFY timeChanged)
public:
    explicit KclockFormat(QObject *parent = nullptr);
    Q_INVOKABLE QString formatTimeString(int hours, int minutes);
    Q_INVOKABLE bool isChecked(int dayIndex, int daysOfWeek);
    QString currentTime(){
        return m_currentTime;
    };

private slots:
    void updateTime();
signals:
    void timeChanged();
    void startDayChanged();
private:
    QTimer* m_timer;
    WeekModel* m_weekModel;
    QString m_currentTime;
};

using weekListItem = std::array<std::tuple<QString, int>, 7>;
class WeekModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum {
        NameRole = Qt::DisplayRole,
        FlagRole = Qt::UserRole + 1};
    explicit WeekModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
private:
    weekListItem m_listItem;
};

#endif // KCLOCKFORMAT_H
