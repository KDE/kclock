/*
 * SPDX-FileCopyrightText: 2025 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "windowexposure.h"

#include <QDebug>
#include <QWindow>

WindowExposure::WindowExposure(QWindow *window, QObject *parent)
    : QObject(parent)
    , m_window(window)
{
    m_exposed = window->isExposed();
    window->installEventFilter(this);
}

WindowExposure *WindowExposure::qmlAttachedProperties(QObject *object)
{
    auto *window = qobject_cast<QWindow *>(object);
    if (!window) {
        qWarning() << "WindowExposure must be attached to a Window";
        return nullptr;
    }
    return new WindowExposure(window, object);
}

bool WindowExposure::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Expose) {
        if (m_window->isExposed() != m_exposed) {
            m_exposed = m_window->isExposed();
            Q_EMIT exposedChanged();
        }
    }

    return QObject::eventFilter(watched, event);
}

bool WindowExposure::isExposed() const
{
    return m_exposed;
}
