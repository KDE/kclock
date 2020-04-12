/*
 * Copyright 2019  Nick Reitemeyer <nick.reitemeyer@web.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ALARMSMODEL_H
#define ALARMSMODEL_H

#include <QAbstractListModel>
#include <QUuid>

class Alarm : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY onPropertyChanged)
    Q_PROPERTY(bool enabled READ getEnabled WRITE setEnabled NOTIFY onPropertyChanged)
    Q_PROPERTY(bool repeated READ getRepeated WRITE setRepeated NOTIFY onPropertyChanged)
public:
    explicit Alarm(QObject* parent = nullptr);

    QString getName() const { return name; }
    bool getEnabled() const { return enabled; }
    bool getRepeated() const { return repeated; }
    QUuid getUuid() { return uuid; }
    void setName(QString name) { this->name = name; changeProperty(); }
    void setRepeated(bool repeated) { this->repeated = repeated; changeProperty(); }
    void setEnabled(bool enabled) { this->enabled = enabled; changeProperty(); }
    bool isDirty() { return dirty; }
    QString getCronString();
    
signals:
    void onPropertyChanged();

private:
    void changeProperty() { dirty = true; emit onPropertyChanged(); }
    
    QString name;
    bool enabled;
    bool repeated;
    bool dirty = false;
    QUuid uuid;
};

class AlarmModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AlarmModel(QObject *parent = nullptr) : QAbstractListModel (parent) { }

    enum {
        NameRole = Qt::DisplayRole,
        RepeatedRole = Qt::UserRole + 0,
        EnabledRole = Qt::UserRole + 1
    };

    int rowCount(const QModelIndex & parent) const override;
    QVariant data(const QModelIndex & index, int role) const override;
    bool setData(const QModelIndex & index, const QVariant & value, int role) override;
    Qt::ItemFlags flags(const QModelIndex & index) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE Alarm* get(int index) { return mList.at(index); }
    Q_INVOKABLE Alarm* addAlarm();
    bool load();
    Q_INVOKABLE bool save();
    QString getCrontabUpdate(const QString crontab);

private:
    QList<Alarm*> mList;
};

#endif // ALARMSMODEL_H
