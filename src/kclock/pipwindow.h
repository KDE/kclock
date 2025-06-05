/*
 * SPDX-FileCopyrightText: 2025 David Edmundson <kde@davidedmundson.co.uk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QQuickWindow>

class PipWindowPrivate;

class PipWindow : public QQuickWindow
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool supported READ isSupported CONSTANT)
public:
    explicit PipWindow();
    ~PipWindow();

    bool isSupported() const;

    bool event(QEvent *event) override;

private:
    QScopedPointer<PipWindowPrivate> d;
};
