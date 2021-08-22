#ifndef KCLOCK_TIMEZONEVIEWPROXYMODEL_H
#define KCLOCK_TIMEZONEVIEWPROXYMODEL_H

#include <QAbstractListModel>
class TimeZoneSelectorModel;
class TimeZoneViewProxyModel : public QAbstractListModel
{
    Q_OBJECT
public:
    TimeZoneViewProxyModel(TimeZoneSelectorModel *parent);
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    TimeZoneSelectorModel *m_parent;
};

#endif // KCLOCK_TIMEZONEVIEWPROXYMODEL_H
