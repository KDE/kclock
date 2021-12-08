/*
 * Copyright 2021 Swapnil Tripathi <swapnil06.st@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "timerpresetmodel.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

const QString TIMERPRESETS_CFG_GROUP = QStringLiteral("TimerPresets"), TIMERPRESETS_CFG_KEY = QStringLiteral("timerPresets");

TimerPreset::TimerPreset(QObject *parent, const QString &presetName, int presetDuration)
    : QObject(parent)
    , m_presetName(presetName)
    , m_presetDuration(presetDuration)
{
}
TimerPreset::TimerPreset(const QJsonObject &obj)
    : m_presetName(obj[QStringLiteral("presetName")].toString())
    , m_presetDuration(obj[QStringLiteral("presetDuration")].toInt())
{
}

TimerPreset::~TimerPreset()
{
}

QJsonObject TimerPreset::toJson() const
{
    QJsonObject obj;
    obj[QStringLiteral("presetName")] = m_presetName;
    obj[QStringLiteral("presetDuration")] = m_presetDuration;
    return obj;
}

QString TimerPreset::presetName() const
{
    return m_presetName;
}

int TimerPreset::presetDuration() const
{
    return m_presetDuration;
}

void TimerPreset::setPresetName(const QString &presetName)
{
    m_presetName = presetName;
    Q_EMIT propertyChanged();
}

void TimerPreset::setDurationLength(int presetDuration)
{
    m_presetDuration = presetDuration;
    Q_EMIT propertyChanged();
}

/* - TimerPresetModel - */

TimerPresetModel *TimerPresetModel::instance()
{
    static TimerPresetModel *s_presetModel = new TimerPresetModel(qApp);
    return s_presetModel;
}

TimerPresetModel::TimerPresetModel(QObject *parent)
    : QAbstractListModel(parent)
{
    load();
}

TimerPresetModel::~TimerPresetModel()
{
    save();

    qDeleteAll(m_presets);
}

void TimerPresetModel::load()
{
    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(TIMERPRESETS_CFG_GROUP);
    QJsonDocument doc = QJsonDocument::fromJson(group.readEntry(TIMERPRESETS_CFG_KEY, "{}").toUtf8());

    const auto array = doc.array();
    std::transform(array.begin(), array.end(), std::back_inserter(m_presets), [](const QJsonValue &pre) {
        return new TimerPreset(pre.toObject());
    });
}

void TimerPresetModel::save()
{
    QJsonArray arr;

    const auto presets = qAsConst(m_presets);
    std::transform(presets.begin(), presets.end(), std::back_inserter(arr), [](const TimerPreset *preset) {
        return QJsonValue(preset->toJson());
    });

    auto config = KSharedConfig::openConfig();
    KConfigGroup group = config->group(TIMERPRESETS_CFG_GROUP);
    group.writeEntry(TIMERPRESETS_CFG_KEY, QString::fromStdString(QJsonDocument(arr).toJson(QJsonDocument::Compact).toStdString()));

    group.sync();
}

QHash<int, QByteArray> TimerPresetModel::roleNames() const
{
    return {{Roles::TimerPresetRole, "preset"}};
}

QVariant TimerPresetModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_presets.count() || index.row() < 0)
        return {};

    auto *preset = m_presets.at(index.row());
    if (role == Roles::TimerPresetRole)
        return QVariant::fromValue(preset);

    return {};
}

int TimerPresetModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_presets.count();
}

void TimerPresetModel::insertPreset(QString presetName, int presetDuration)
{
    Q_EMIT beginInsertRows({}, 0, 0);
    m_presets.insert(0, new TimerPreset(this, presetName, presetDuration));
    Q_EMIT endInsertRows();

    save();
}

void TimerPresetModel::deletePreset(const int index)
{
    beginRemoveRows({}, index, index);
    m_presets.removeAt(index);
    endRemoveRows();

    save();
}
