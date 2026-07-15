/*
 * Copyright 2022 Devin Lin <devin@kde.org>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QObject>
#include <optional>

class XDGPortal : public QObject
{
    Q_OBJECT

public:
    explicit XDGPortal(QObject *parent = nullptr);

    void setAutostart(bool autostart);

Q_SIGNALS:
    void requestStarted();
    void requestFinished();

private Q_SLOTS:
    void requestBackgroundResponse(uint response, const QVariantMap &results);

private:
    void requestBackground();
    void finishRequest();

    std::optional<bool> m_lastRequestedAutostart;
    bool m_desiredAutostart = false;
    bool m_requestAutostart = false;
    bool m_requestPending = false;
    uint m_handleToken = 0;
    QString m_requestPath;
};
