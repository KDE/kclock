/*
    SPDX-License-Identifier: GPL-3.0-or-later
    SPDX-FileCopyrightText: 2022 Felipe Kinoshita <kinofhek@gmail.com>
*/

#pragma once

#include <QObject>
#include <KAboutData>

class AboutType : public QObject
{
    Q_OBJECT
    Q_PROPERTY(KAboutData aboutData READ aboutData CONSTANT)
public:
    static AboutType &instance()
    {
        static AboutType _instance;
        return _instance;
    }

    [[nodiscard]] KAboutData aboutData() const
    {
        return KAboutData::applicationData();
    }
};
