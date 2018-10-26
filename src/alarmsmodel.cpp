#include "alarmsmodel.h"

Alarm::Alarm(QObject* parent) : QObject(parent)
{
    name = "test";
    enabled = true;
}

int AlarmModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return mList.size();
}

QVariant AlarmModel::data(const QModelIndex & index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        return mList.at(index.row())->getName();
    case EnabledRole:
        return mList.at(index.row())->getEnabled();
    default:
        return QVariant();
    }
}

bool AlarmModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid()) {
        switch (role) {
        case NameRole:
            if (value.type() == QVariant::String) {
                mList[index.row()]->setName(value.toString());
                emit dataChanged(index, index, QVector<int> { NameRole });
                return true;
            }
            return false;
        case EnabledRole:
            if(value.type() == QVariant::Bool) {
                mList[index.row()]->setEnabled(value.toBool());
                emit dataChanged(index, index, QVector<int> { EnabledRole });
                return true;
            }
        }
    }
    return false;
}

Qt::ItemFlags AlarmModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEditable;
}

QHash<int, QByteArray> AlarmModel::roleNames() const
{
    auto roles = QHash<int, QByteArray>();
    roles[NameRole] = "name";
    roles[EnabledRole] = "enabled";
    roles[RepeatedRole] = "repeated";
    return roles;
}

void AlarmModel::addAlarm()
{
    beginInsertRows(QModelIndex(), rowCount(QModelIndex()), rowCount(QModelIndex()));
    mList.append(new Alarm(this));
    endInsertRows();
}

