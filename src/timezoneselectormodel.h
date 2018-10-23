#ifndef TIMEZONESELECTORMODEL_H
#define TIMEZONESELECTORMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QTimeZone>

class TimeZoneSelectorModel : public QAbstractListModel
{
    Q_OBJECT
    
public:
    explicit TimeZoneSelectorModel(QObject *parent = nullptr);
    
    enum {
        NameRole = Qt::DisplayRole,
        ShownRole
    };
    
    int rowCount(const QModelIndex & parent) const override;
    QVariant data(const QModelIndex & index, int role) const override;
    bool setData(const QModelIndex & index, const QVariant & value, int role) override;
    Qt::ItemFlags flags(const QModelIndex & index) const override;
    QHash<int, QByteArray> roleNames() const override;
    
private:
    QList<std::tuple<QTimeZone, bool>> mList;
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
