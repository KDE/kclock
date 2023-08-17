
# Copyright 2020 Han Young <hanyoung@protonmail.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.kclock_1x2\")

kcoreaddons_add_plugin(org.kde.plasma.kclock_1x2 SOURCES kclock_1x2.cpp INSTALL_NAMESPACE plasma/applets)

target_link_libraries(org.kde.plasma.kclock_1x2
    Qt::Gui
    Qt::DBus
    KF${QT_MAJOR_VERSION}::Plasma
    KF${QT_MAJOR_VERSION}::I18n
)

install(FILES kclock_plasmoid_1x2.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps/)
plasma_install_package(package org.kde.plasma.kclock_1x2)
