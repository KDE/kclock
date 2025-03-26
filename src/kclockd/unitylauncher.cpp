/*
 * SPDX-FileCopyrightText: 2025 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "unitylauncher.h"

#include <QDBusConnection>
#include <QDBusMessage>

#include <QtNumeric>

#include <cmath>

UnityLauncher::UnityLauncher(QObject *parent)
    : QObject(parent)
{
    // Clear progress initially in case previous instance didn't properly clean up (crash).
    setProgress(qQNaN());
}

UnityLauncher::~UnityLauncher()
{
    setProgress(qQNaN());
}

void UnityLauncher::setProgress(qreal progress)
{
    const bool oldHasProgress = !std::isnan(m_progress);
    const bool hasProgress = !std::isnan(progress);

    // NaN doesn't equal itself, hence isEmpty check below.
    if (m_progress == progress) {
        return;
    }

    QVariantMap properties;

    // Spec requires progress be a double.
    if (hasProgress) {
        properties.insert(QStringLiteral("progress"), double(progress));
    }

    if (oldHasProgress != hasProgress) {
        properties.insert(QStringLiteral("progress-visible"), hasProgress);
    }

    if (properties.isEmpty()) {
        return;
    }

    QDBusMessage msg =
        QDBusMessage::createSignal(QStringLiteral("/org/kde/KClock"), QStringLiteral("com.canonical.Unity.LauncherEntry"), QStringLiteral("Update"));

    msg.setArguments({QStringLiteral("applications://org.kde.kclock"), properties});

    QDBusConnection::sessionBus().send(msg);

    m_progress = progress;
}

#include "moc_unitylauncher.cpp"
