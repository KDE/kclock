/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

namespace KClock
{
template<class C>
int insert_index(const typename C::value_type &obj,
                 const C &container,
                 bool (*less_than)(const typename C::value_type &left, const typename C::value_type &right))
{
    int index = 0;
    for (auto element : container) {
        if (less_than(obj, element)) {
            return index;
        }
        ++index;
    }
    return index;
};
}
