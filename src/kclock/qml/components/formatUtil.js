/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020-2021 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// display format for days of the week to repeat for alarms
function getRepeatFormat(dayOfWeek) {
    if (dayOfWeek == 0) {
        return i18n("Only once");
    }
    let monday = 1 << 0, tuesday = 1 << 1, wednesday = 1 << 2, thursday = 1 << 3, friday = 1 << 4, saturday = 1 << 5, sunday = 1 << 6;

    if (dayOfWeek == monday + tuesday + wednesday + thursday + friday + saturday + sunday)
        return i18n("Everyday");

    if (dayOfWeek == monday + tuesday + wednesday + thursday + friday)
        return i18n("Weekdays");

    let str = "";
    if (dayOfWeek & monday) str += i18n("Mon, ");
    if (dayOfWeek & tuesday) str += i18n("Tue, ");
    if (dayOfWeek & wednesday) str += i18n("Wed, ");
    if (dayOfWeek & thursday) str += i18n("Thu, ");
    if (dayOfWeek & friday) str += i18n("Fri, ");
    if (dayOfWeek & saturday) str += i18n("Sat, ");
    if (dayOfWeek & sunday) str += i18n("Sun, ");
    return str.substring(0, str.length - 2);
}

// auxiliary function to convert 24 hours to 12
function hoursTo12(hours) {
    return (hours >= 12) ? (hours - 12) : hours;
}
