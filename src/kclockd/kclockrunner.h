/*
 * SPDX-FileCopyrightText: 2025 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "dbusutils_p.h"

#include <QDBusArgument>
#include <QDBusContext>
#include <QDBusMessage>
#include <QObject>
#include <QString>

class KClockRunner : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kclock.KClockRunner")
public:
    explicit KClockRunner(QObject *parent = nullptr);

    void Teardown();
    RemoteActions Actions() const;
    RemoteMatches Match(const QString &searchTerm);
    void Run(const QString &id, const QString &actionId);
};
