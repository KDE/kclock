/*
    SPDX-FileCopyrightText: 2017, 2018 David Edmundson <davidedmundson@kde.org>
    SPDX-FileCopyrightText: 2020 Alexander Lohnau <alexander.lohnau@gmx.de>
    SPDX-FileCopyrightText: 2020 Kai Uwe Broulik <kde@broulik.de>

 SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QDBusArgument>
#include <QList>
#include <QString>
#include <QVariantMap>

const qreal HighestCategoryRelevance = 100; // KRunner::QueryMatch::CategoryRelevance::Highest

struct RemoteMatch {
    // sssida{sv}
    QString id;
    QString text;
    QString iconName;
    int categoryRelevance = HighestCategoryRelevance;
    qreal relevance = 0;
    QVariantMap properties;
};

using RemoteMatches = QList<RemoteMatch>;

inline QDBusArgument &operator<<(QDBusArgument &argument, const RemoteMatch &match)
{
    argument.beginStructure();
    argument << match.id;
    argument << match.text;
    argument << match.iconName;
    argument << match.categoryRelevance;
    argument << match.relevance;
    argument << match.properties;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, RemoteMatch &match)
{
    argument.beginStructure();
    argument >> match.id;
    argument >> match.text;
    argument >> match.iconName;
    argument >> match.categoryRelevance;
    argument >> match.relevance;
    argument >> match.properties;
    argument.endStructure();

    return argument;
}

struct RemoteAction {
    QString id;
    QString text;
    QString iconSource;
};

using RemoteActions = QList<RemoteAction>;

inline QDBusArgument &operator<<(QDBusArgument &argument, const RemoteAction &action)
{
    argument.beginStructure();
    argument << action.id;
    argument << action.text;
    argument << action.iconSource;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, RemoteAction &action)
{
    QString id;
    QString text;
    QString iconSource;
    argument.beginStructure();
    argument >> id;
    argument >> text;
    argument >> iconSource;
    argument.endStructure();
    action = RemoteAction{id, iconSource, text};
    return argument;
}

Q_DECLARE_METATYPE(RemoteAction)
Q_DECLARE_METATYPE(RemoteActions)
Q_DECLARE_METATYPE(RemoteMatch)
Q_DECLARE_METATYPE(RemoteMatches)
