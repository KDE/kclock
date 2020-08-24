# TODO: adapt "org.kde.plasma" here & elsewhere if needed (see README)
add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.kclock_1x2\")

set(kclock_1x2_SRCS
    kclock_1x2.cpp
)
add_library(plasma_applet_kclock_1x2 MODULE ${kclock_1x2_SRCS})

kcoreaddons_desktop_to_json(plasma_applet_kclock_1x2 package/metadata.desktop SERVICE_TYPES plasma-applet.desktop)

target_link_libraries(plasma_applet_kclock_1x2
                      Qt5::Gui
                      Qt5::DBus
                      KF5::Plasma
                      KF5::I18n)


install(TARGETS plasma_applet_kclock_1x2 DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/applets)

plasma_install_package(package org.kde.plasma.kclock_1x2)
