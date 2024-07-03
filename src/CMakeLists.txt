#
# Copyright 2020 Han Young <hanyoung@protonmail.com>
# Copyright 2020 Devin Lin <espidev@gmail.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

add_subdirectory(kclockd)
add_subdirectory(kclock)
if (BUILD_PLASMOID)
    add_subdirectory(plasmoid)
endif()
