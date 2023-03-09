#
# Copyright 2020 Han Young <hanyoung@protonmail.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

# TODO: adapt "org.kde.plasma" here & elsewhere if needed (see README)
add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.kclock_kweather_3x3\")

set(kclock_kweather_3x3_SRCS
    kclock_kweather_3x3.cpp
)
add_library(plasma_applet_kclock_kweather_3x3 MODULE ${kclock_kweather_3x3_SRCS})

target_link_libraries(plasma_applet_kclock_kweather_3x3
                      Qt5::Gui
                      Qt5::DBus
                      KF${QT_MAJOR_VERSION}::Plasma
                      KF${QT_MAJOR_VERSION}::I18n)


install(TARGETS plasma_applet_kclock_kweather_3x3 DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/applets)

plasma_install_package(package org.kde.plasma.kclock_kweather_3x3)
