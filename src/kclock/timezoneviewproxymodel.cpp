#include "timezoneviewproxymodel.h"
#include "timezoneselectormodel.h"
TimeZoneViewProxyModel::TimeZoneViewProxyModel(TimeZoneSelectorModel *parent)
    : QAbstractListModel((QObject *)parent)
    , m_parent(parent)
{
}

int TimeZoneViewProxyModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_parent->selectedTimeZones().size();
}
QVariant TimeZoneViewProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() > (int)m_parent->selectedTimeZones().size())
        return {};
    auto m_index = createIndex(m_parent->selectedTimeZones()[index.row()], 1);
    return m_parent->data(m_index, role);
}
bool TimeZoneViewProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() > (int)m_parent->selectedTimeZones().size())
        return false;
    auto m_index = createIndex(m_parent->selectedTimeZones()[index.row()], 1);
    return m_parent->setData(m_index, value, role);
}
Qt::ItemFlags TimeZoneViewProxyModel::flags(const QModelIndex &index) const
{
    return m_parent->flags(index);
}
QHash<int, QByteArray> TimeZoneViewProxyModel::roleNames() const
{
    return m_parent->roleNames();
}
