#include <QTimeZone>
#include <QDebug>
#include "timezoneselectormodel.h"

TimeZoneSelectorModel::TimeZoneSelectorModel(QObject* parent) : QAbstractListModel(parent)
{
    for (QByteArray id : QTimeZone::availableTimeZoneIds()) {
        mList.append(std::make_tuple(QTimeZone(id), false));
    }
    mTimer.setInterval(1000);
    connect(&mTimer, &QTimer::timeout, this, &TimeZoneSelectorModel::update);
    mTimer.start(1000);
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
    case IDRole:
        return std::get<0>(tuple).id();
    case ShortNameRole:
        return std::get<0>(tuple).displayName(QDateTime::currentDateTime(), QTimeZone::ShortName);
    case TimeStringRole:
        QDateTime time = QDateTime::currentDateTime();
        time = time.toTimeZone(std::get<0>(tuple));
        return time.time().toString("hh:mm:ss");
    }
    return QVariant();
}

QHash<int, QByteArray> TimeZoneSelectorModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ShownRole] = "shown";
    roles[OffsetRole] = "offset";
    roles[ShortNameRole] = "shortName";
    roles[TimeStringRole] = "timeString";
    roles[IDRole] = "id";
    return roles;
}

void TimeZoneSelectorModel::update()
{
    QVector<int> roles = { TimeStringRole };
    emit dataChanged(index(0), index(mList.size() - 1), roles);
}

Qt::ItemFlags TimeZoneSelectorModel::flags(const QModelIndex& index) const
{
    Q_UNUSED(index)
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
    setFilterRole(TimeZoneSelectorModel::IDRole);
}

TimeZoneViewModel::TimeZoneViewModel(TimeZoneSelectorModel *model, QObject *parent) : QSortFilterProxyModel(parent)
{
    setSourceModel(model);
    setFilterRole(TimeZoneSelectorModel::ShownRole);
    setFilterFixedString("true");
}
