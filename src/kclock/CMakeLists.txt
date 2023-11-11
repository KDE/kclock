#
# Copyright 2020 Han Young <hanyoung@protonmail.com>
# Copyright 2020-2021 Devin Lin <devin@kde.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

set(kclock_SRCS
    alarm.cpp
    alarm.h
    alarmmodel.cpp
    alarmmodel.h
    timer.cpp
    timer.h
    timermodel.cpp
    timermodel.h
    utilmodel.cpp
    utilmodel.h
    stopwatchtimer.cpp
    stopwatchtimer.h
    kclockformat.cpp
    kclockformat.h
    settingsmodel.cpp
    settingsmodel.h
    addlocationmodel.cpp
    addlocationmodel.h
    timerpresetmodel.cpp
    timerpresetmodel.h
    savedlocationsmodel.cpp
    savedlocationsmodel.h
    )

set(SettingsXML ${CMAKE_CURRENT_BINARY_DIR}/../kclockd/org.kde.kclockd.KClockSettings.xml)
set(AlarmXML ${CMAKE_CURRENT_BINARY_DIR}/../kclockd/org.kde.kclockd.Alarm.xml)
set(AlarmModelXML ${CMAKE_CURRENT_BINARY_DIR}/../kclockd/org.kde.kclockd.AlarmModel.xml)
set(TimermModelXML ${CMAKE_CURRENT_BINARY_DIR}/../kclockd/org.kde.kclockd.TimerModel.xml)
set(TimerXML ${CMAKE_CURRENT_BINARY_DIR}/../kclockd/org.kde.kclockd.Timer.xml)
set(UtilityXML ${CMAKE_CURRENT_BINARY_DIR}/../kclockd/org.kde.kclockd.Utility.xml)

qt_add_dbus_interface(kclock_SRCS ${SettingsXML} kclocksettingsinterface )
qt_add_dbus_interface(kclock_SRCS ${AlarmXML} alarminterface )
qt_add_dbus_interface(kclock_SRCS ${AlarmModelXML} alarmmodelinterface )
qt_add_dbus_interface(kclock_SRCS ${TimermModelXML} timermodelinterface )
qt_add_dbus_interface(kclock_SRCS ${TimerXML} timerinterface )
qt_add_dbus_interface(kclock_SRCS ${UtilityXML} utilityinterface )

qt_add_resources(RESOURCES resources.qrc)

add_executable(kclock main.cpp ${kclock_SRCS} ${RESOURCES})
add_dependencies(kclock kclockd)
target_link_libraries(kclock PRIVATE
    Qt6::Qml
    Qt6::Gui
    Qt6::QuickControls2
    Qt6::Widgets
    Qt6::Multimedia
    KF6::I18n
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::CoreAddons
    KF6::Notifications
    KF6::DBusAddons
    KF6::StatusNotifierItem
)

target_include_directories(kclock PRIVATE ${CMAKE_BINARY_DIR})
install(TARGETS kclock ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
