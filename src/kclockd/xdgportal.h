/*
 * Copyright 2022 Devin Lin <devin@kde.org>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QObject>

class XDGPortal : QObject
{
    Q_OBJECT

public:
    void requestBackground();

private Q_SLOTS:
    void requestBackgroundResponse(uint response, const QVariantMap &results);

private:
    QString m_handleToken = QStringLiteral("u1");
};
