#
# Copyright 2020 Han Young <hanyoung@protonmail.com>
# Copyright 2020 Devin Lin <espidev@gmail.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

set(kclockd_SRCS
    utilities.cpp
    alarm.cpp
    alarmmodel.cpp
    alarmwaitworker.cpp
    alarmplayer.cpp
    timermodel.cpp
    timer.cpp
    xdgportal.cpp
    )
# generate KConfig class from xml
kconfig_add_kcfg_files(kclockd_SRCS kclockdsettings.kcfgc GENERATE_MOC)

# use generated KConfig header file to generate DBus adaptor xml
set(kclockd_SRCS
    ${kclockd_SRCS}
    ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.KClockSettings.xml
)

qt_generate_dbus_interface(
    ${CMAKE_CURRENT_BINARY_DIR}/kclockdsettings.h
    org.kde.kclockd.KClockSettings.xml
    OPTIONS -S -P
)
qt_generate_dbus_interface(
    ${CMAKE_CURRENT_SOURCE_DIR}/alarm.h
    org.kde.kclockd.Alarm.xml
    OPTIONS -s -m -P
)
qt_generate_dbus_interface(
    ${CMAKE_CURRENT_SOURCE_DIR}/alarmmodel.h
    org.kde.kclockd.AlarmModel.xml
    OPTIONS -s -m
)
qt_generate_dbus_interface(
    ${CMAKE_CURRENT_SOURCE_DIR}/timermodel.h
    org.kde.kclockd.TimerModel.xml
    OPTIONS -s -m -p
)

qt_generate_dbus_interface(
    ${CMAKE_CURRENT_SOURCE_DIR}/timer.h
    org.kde.kclockd.Timer.xml
    OPTIONS -s -m -P
)

qt_generate_dbus_interface(
    ${CMAKE_CURRENT_SOURCE_DIR}/utilities.h
    org.kde.kclockd.Utility.xml
    OPTIONS -s -m -P
)
# use generated DBus adapator xml to generate adaptor source code
qt_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.KClockSettings.xml
                     ${CMAKE_CURRENT_BINARY_DIR}/kclockdsettings.h KClockSettings)
qt_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Alarm.xml
                     ${CMAKE_CURRENT_SOURCE_DIR}/alarm.h Alarm)
qt_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.AlarmModel.xml
                     ${CMAKE_CURRENT_SOURCE_DIR}/alarmmodel.h AlarmModel)
qt_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.TimerModel.xml
                     ${CMAKE_CURRENT_SOURCE_DIR}/timermodel.h TimerModel)
qt_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Timer.xml
                     ${CMAKE_CURRENT_SOURCE_DIR}/timer.h Timer)
qt_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Utility.xml
                     ${CMAKE_CURRENT_SOURCE_DIR}/utilities.h Utilities)

# install DBus interface xml to dbus interface directory
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.KClockSettings.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Alarm.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.AlarmModel.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.TimerModel.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Timer.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Utility.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})

add_executable(kclockd main.cpp ${kclockd_SRCS} )
target_link_libraries(kclockd
    Qt::Multimedia
    KF${QT_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::ConfigCore
    KF${QT_MAJOR_VERSION}::ConfigGui
    KF${QT_MAJOR_VERSION}::CoreAddons
    KF${QT_MAJOR_VERSION}::Notifications
    KF${QT_MAJOR_VERSION}::DBusAddons
    )
target_include_directories(kclockd PRIVATE ${CMAKE_BINARY_DIR})
install(TARGETS kclockd ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
