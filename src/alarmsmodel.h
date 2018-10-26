#ifndef ALARMSMODEL_H
#define ALARMSMODEL_H

#include <QAbstractListModel>

class Alarm : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ getName WRITE setName)
    Q_PROPERTY(bool enabled READ getEnabled WRITE setEnabled)
    Q_PROPERTY(bool repeated READ getRepeated WRITE setRepeated)
public:
    explicit Alarm(QObject* parent = nullptr);

    QString getName() const { return name; }
    bool getEnabled() const { return enabled; }
    bool getRepeated() const { return repeated; }
    void setName(QString name) { this->name = name; }
    void setRepeated(bool repeated) { this->repeated = repeated; }
    void setEnabled(bool enabled) { this->enabled = enabled; }

private:
    QString name;
    bool enabled;
    bool repeated;
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
    Q_INVOKABLE void addAlarm();

private:
    QList<Alarm*> mList;
};

#endif // ALARMSMODEL_H
