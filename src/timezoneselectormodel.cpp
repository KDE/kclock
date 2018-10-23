#include <QTimeZone>
#include <QDebug>
#include "timezoneselectormodel.h"

TimeZoneSelectorModel::TimeZoneSelectorModel(QObject* parent) : QAbstractListModel(parent)
{
    for (QByteArray id : QTimeZone::availableTimeZoneIds()) {
        mList.append(std::make_tuple(QTimeZone(id), false));
    }
}


int TimeZoneSelectorModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mList.count();
}

QVariant TimeZoneSelectorModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();
    auto tuple = mList[index.row()];
    switch(role) {
    case NameRole:
        return std::get<0>(tuple).displayName(QDateTime::currentDateTime());
    case ShownRole:
        return std::get<1>(tuple);
    case OffsetRole:
        return std::get<0>(tuple).displayName(QDateTime::currentDateTime(), QTimeZone::OffsetName);
    case ShortNameRole:
        return std::get<0>(tuple).displayName(QDateTime::currentDateTime(), QTimeZone::ShortName);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> TimeZoneSelectorModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ShownRole] = "shown";
    roles[OffsetRole] = "offsetName";
    roles[ShortNameRole] = "shortName";
    return roles;
}

Qt::ItemFlags TimeZoneSelectorModel::flags(const QModelIndex& index) const
{
    return Qt::ItemIsEditable;
}

bool TimeZoneSelectorModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(index.isValid() && role == ShownRole && value.type() == QVariant::Bool) {
        std::get<1>(mList[index.row()]) = value.toBool();
        emit dataChanged(index, index, QVector<int> { ShownRole });
        return true;
    }
    return false;
}

TimeZoneFilterModel::TimeZoneFilterModel(TimeZoneSelectorModel *model, QObject *parent) : QSortFilterProxyModel (parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setSourceModel(model);
}

TimeZoneViewModel::TimeZoneViewModel(TimeZoneSelectorModel *model, QObject *parent) : QSortFilterProxyModel(parent)
{
    setSourceModel(model);
    setFilterRole(TimeZoneSelectorModel::ShownRole);
    setFilterFixedString("true");
}
