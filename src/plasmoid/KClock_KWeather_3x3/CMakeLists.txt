
# Copyright 2020 Han Young <hanyoung@protonmail.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.kclock_kweather_3x3\")

kcoreaddons_add_plugin(plasma_applet_kclock_kweather_3x3 SOURCES kclock_3x3.cpp INSTALL_NAMESPACE plasma/applets)

target_link_libraries(plasma_applet_kclock_kweather_3x3
    Qt6::Gui
    Qt6::DBus
    Plasma::Plasma
    KF6::I18n
)

plasma_install_package(package org.kde.plasma.kclock_kweather_3x3)
