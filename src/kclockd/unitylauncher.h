/*
 * SPDX-FileCopyrightText: 2025 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QObject>

class UnityLauncher : public QObject
{
    Q_OBJECT
public:
    explicit UnityLauncher(QObject *parent = nullptr);
    ~UnityLauncher() override;

    void setProgress(qreal progress);

private:
    qreal m_progress = 0;
};
