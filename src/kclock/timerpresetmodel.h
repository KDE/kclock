// SPDX-FileCopyrightText: 2021 Swapnil Tripathi <swapnil06.st@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef TIMERPRESETMODEL_H
#define TIMERPRESETMODEL_H

#include <QAbstractListModel>
#include <QCoreApplication>
#include <QJsonObject>
#include <QObject>
#include <QSettings>

class TimerPreset : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString presetName READ presetName WRITE setPresetName NOTIFY propertyChanged)
    Q_PROPERTY(int presetDuration READ presetDuration NOTIFY propertyChanged)
public:
    explicit TimerPreset(QObject *parent = nullptr, const QString &presetName = {}, int presetDuration = 0);
    explicit TimerPreset(const QJsonObject &obj);

    ~TimerPreset();

    QJsonObject toJson() const;

    QString presetName() const
    {
        return m_presetName;
    }

    int presetDuration() const
    {
        return m_presetDuration;
    }

    void setPresetName(const QString &presetName);

    void setDurationLength(int presetDuration);

private:
    QString m_presetName;
    int m_presetDuration;
signals:
    void propertyChanged();
};

class TimerPresetModel;
static TimerPresetModel *s_presetModel = nullptr;

class TimerPresetModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles { TimerPresetRole = Qt::UserRole };
    static TimerPresetModel *instance()
    {
        if (!s_presetModel) {
            s_presetModel = new TimerPresetModel(qApp);
        }
        return s_presetModel;
    }

    void load();

    void save();

    QHash<int, QByteArray> roleNames() const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE void insertPreset(QString presetName, int presetDuration);
    Q_INVOKABLE void deletePreset(const int index);

private:
    explicit TimerPresetModel(QObject *parent = nullptr);
    ~TimerPresetModel();

    QSettings *m_settings;
    QList<TimerPreset *> m_presets;
};

#endif // TIMERPRESETMODEL_H
