#
# Copyright 2020 Han Young <hanyoung@protonmail.com>
# Copyright 2020 Devin Lin <espidev@gmail.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

set(kclockd_SRCS
    utilities.cpp
    utilities.h
    alarm.cpp
    alarm.h
    alarmmodel.cpp
    alarmmodel.h
    alarmwaitworker.cpp
    alarmwaitworker.h
    alarmplayer.cpp
    alarmplayer.h
    kclockrunner.cpp
    kclockrunner.h
    timermodel.cpp
    timermodel.h
    timer.cpp
    timer.h
    xdgportal.cpp
    xdgportal.h
    abstractwakeupprovider.h
    powerdevilwakeupprovider.cpp
    powerdevilwakeupprovider.h
    unitylauncher.cpp
    unitylauncher.h
    waittimerwakeupprovider.cpp
    waittimerwakeupprovider.h
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
                     kclockdsettings.h KClockSettings)
qt_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Alarm.xml
                     alarm.h Alarm)
qt_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.AlarmModel.xml
                     alarmmodel.h AlarmModel)
qt_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.TimerModel.xml
                     timermodel.h TimerModel)
qt_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Timer.xml
                     timer.h Timer)
qt_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Utility.xml
                     utilities.h Utilities)

qt_add_dbus_interface(kclockd_SRCS systeminterfaces/org.mpris.MediaPlayer2.Player.xml generated/systeminterfaces/mprisplayer)
qt_add_dbus_adaptor(kclockd_SRCS systeminterfaces/org.kde.krunner1.xml kclockrunner.h KClockRunner)

# install DBus interface xml to dbus interface directory
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.KClockSettings.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Alarm.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.AlarmModel.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.TimerModel.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Timer.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.Utility.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR})

add_executable(kclockd main.cpp ${kclockd_SRCS} )
target_link_libraries(kclockd PRIVATE
    Qt6::Multimedia
    Qt6::Widgets
    KF6::I18n
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::CoreAddons
    KF6::Notifications
    KF6::DBusAddons
    KF6::StatusNotifierItem
    KF6::Crash
)

ecm_qt_declare_logging_category(kclockd HEADER debug_runner.h
    IDENTIFIER RUNNER_DEBUG
    CATEGORY_NAME org.kde.kclock.runner
    DESCRIPTION "KClock Runner"
)

target_include_directories(kclockd PRIVATE ${CMAKE_BINARY_DIR})
install(TARGETS kclockd ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
