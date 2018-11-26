#ifndef TIMEZONESELECTORMODEL_H
#define TIMEZONESELECTORMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QTimeZone>
#include <QTimer>

class TimeZoneSelectorModel : public QAbstractListModel
{
    Q_OBJECT
    
public:
    explicit TimeZoneSelectorModel(QObject *parent = nullptr);
    
    enum Roles {
        NameRole = Qt::DisplayRole,
        ShownRole = Qt::UserRole + 0,
        OffsetRole = Qt::UserRole + 1,
        ShortNameRole = Qt::UserRole + 2,
        TimeStringRole,
        IDRole
    };
    
    int rowCount(const QModelIndex & parent) const override;
    QVariant data(const QModelIndex & index, int role) const override;
    bool setData(const QModelIndex & index, const QVariant & value, int role) override;
    Qt::ItemFlags flags(const QModelIndex & index) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void update();
    
private:
    QList<std::tuple<QTimeZone, bool>> mList;
    QTimer mTimer;
};

class TimeZoneFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit TimeZoneFilterModel(TimeZoneSelectorModel *model, QObject *parent = nullptr);
};

class TimeZoneViewModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit TimeZoneViewModel(TimeZoneSelectorModel *model, QObject *parent = nullptr);
};

#endif
