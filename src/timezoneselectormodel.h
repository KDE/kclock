#ifndef TIMEZONESELECTORMODEL_H
#define TIMEZONESELECTORMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

class TimeZoneSelectorModel : public QAbstractListModel
{
    Q_OBJECT
    
public:
    explicit TimeZoneSelectorModel(QObject *parent = nullptr);
    
    class TimeZoneData {
    public:
        TimeZoneData(QString name, bool shown) {
            this->name = name;
            this->shown = shown;
        }
        QString name;
        bool shown;
    };
    
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
    QList<TimeZoneData> mList;
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
