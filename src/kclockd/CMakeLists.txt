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

qt5_generate_dbus_interface(
    ${CMAKE_CURRENT_BINARY_DIR}/kclockdsettings.h
    org.kde.kclockd.KClockSettings.xml
    OPTIONS -S -P
)
qt5_generate_dbus_interface(
    ${CMAKE_CURRENT_SOURCE_DIR}/alarm.h
    org.kde.kclockd.Alarm.xml
    OPTIONS -s -m -P
)
qt5_generate_dbus_interface(
    ${CMAKE_CURRENT_SOURCE_DIR}/alarmmodel.h
    org.kde.kclockd.AlarmModel.xml
    OPTIONS -s -m
)
qt5_generate_dbus_interface(
    ${CMAKE_CURRENT_SOURCE_DIR}/timermodel.h
    org.kde.kclockd.TimerModel.xml
    OPTIONS -s -m -p
)

qt5_generate_dbus_interface(
    ${CMAKE_CURRENT_SOURCE_DIR}/timer.h
    org.kde.kclockd.Timer.xml
    OPTIONS -s -m -P
)

qt5_generate_dbus_interface(
    ${CMAKE_CURRENT_SOURCE_DIR}/utilities.h
    org.kde.kclockd.Utility.xml
    OPTIONS -s -m -P
)
# use generated DBus adapator xml to generate adaptor source code
qt5_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.KClockSettings.xml
                     ${CMAKE_CURRENT_BINARY_DIR}/kclockdsettings.h KClockSettings)
qt5_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Alarm.xml
                     ${CMAKE_CURRENT_SOURCE_DIR}/alarm.h Alarm)
qt5_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.AlarmModel.xml
                     ${CMAKE_CURRENT_SOURCE_DIR}/alarmmodel.h AlarmModel)
qt5_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.TimerModel.xml
                     ${CMAKE_CURRENT_SOURCE_DIR}/timermodel.h TimerModel)
qt5_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Timer.xml
                     ${CMAKE_CURRENT_SOURCE_DIR}/timer.h Timer)
qt5_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Utility.xml
                     ${CMAKE_CURRENT_SOURCE_DIR}/utilities.h Utilities)

# install DBus interface xml to dbus interface directory
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.KClockSettings.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Alarm.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.AlarmModel.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.TimerModel.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Timer.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Utility.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})

add_executable(kclockd main.cpp ${kclockd_SRCS} )
target_link_libraries(kclockd
    Qt5::Multimedia
    KF5::I18n
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::CoreAddons
    KF5::Notifications
    KF5::DBusAddons
    )
target_include_directories(kclockd PRIVATE ${CMAKE_BINARY_DIR})
install(TARGETS kclockd ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
