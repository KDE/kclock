/*
 * SPDX-FileCopyrightText: 2025 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QObject>
#include <qqmlintegration.h>

class QWindow;

/**
 * This class tracks the isExposed state of a QWindow.
 *
 * Under Wayland, a window is either exposed or it is not.
 * There's no real concept such as minimized or hidden or anything.
 */
class WindowExposure : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Can only be used as attached property")
    QML_ATTACHED(WindowExposure)

    Q_PROPERTY(bool exposed READ isExposed NOTIFY exposedChanged)

public:
    explicit WindowExposure(QWindow *window, QObject *parent = nullptr);

    bool isExposed() const;
    Q_SIGNAL void exposedChanged();

    bool eventFilter(QObject *watched, QEvent *event) override;

    static WindowExposure *qmlAttachedProperties(QObject *object);

private:
    QWindow *m_window;
    bool m_exposed = false;
};
