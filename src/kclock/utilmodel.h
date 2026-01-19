// SPDX-FileCopyrightText: 2020 Han Young <hanyoung@protonmail.com>
// SPDX-FileCopyrightText: 2020-2024 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <QString>

#include <KSvg/ImageSet>

#include <qqmlintegration.h>

class QJSEngine;
class QQmlEngine;

namespace KSvg
{
class Svg;
};

class UtilModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString tzName READ getCurrentTimeZoneName CONSTANT)
    Q_PROPERTY(bool use24HourTime READ use24HourTime NOTIFY use24HourTimeChanged)

public:
    static UtilModel *instance();
    static UtilModel *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    Q_INVOKABLE bool systemHasPlasmaMobileSoundTheme();
    Q_INVOKABLE bool systemHasOceanSoundTheme();
    Q_INVOKABLE QString getDefaultAlarmFileLocation();

    QString getCurrentTimeZoneName();

    long long calculateNextRingTime(int hours, int minutes, int daysOfWeek, int snooze = 0);
    QString timeToRingFormatted(const long long &timestamp);

    QString timeFormat() const;
    bool use24HourTime() const;

    // use to turn milliseconds -> hh:mm:ss.00
    Q_INVOKABLE qint64 msToHoursPart(qint64 ms) const;
    Q_INVOKABLE qint64 msToMinutesPart(qint64 ms) const;
    Q_INVOKABLE qint64 msToSecondsPart(qint64 ms) const;
    Q_INVOKABLE qint64 msToSmallPart(qint64 ms) const; // hundredths of second
    Q_INVOKABLE QString displayTwoDigits(const qint64 &amount);
    Q_INVOKABLE QString repeatFormat(int dayOfWeek) const;

    Q_INVOKABLE void applyPlasmaImageSet(KSvg::Svg *svg);

private:
    explicit UtilModel(QObject *parent = nullptr);

    bool isLocale24HourTime() const;

Q_SIGNALS:
    void use24HourTimeChanged();

private:
    KSvg::ImageSet *m_plasmaImageSet = nullptr;
};
