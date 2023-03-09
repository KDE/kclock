#
# Copyright 2020 Han Young <hanyoung@protonmail.com>
# Copyright 2020-2021 Devin Lin <devin@kde.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

set(kclock_SRCS
    about.cpp
    alarm.cpp
    alarmmodel.cpp
    timer.cpp
    timermodel.cpp
    utilmodel.cpp
    stopwatchtimer.cpp
    kclockformat.cpp
    settingsmodel.cpp
    addlocationmodel.cpp
    timerpresetmodel.cpp
    savedlocationsmodel.cpp
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

if(QT_MAJOR_VERSION STREQUAL "5")
    qtquick_compiler_add_resources(RESOURCES resources.qrc)
else()
    qt_add_resources(RESOURCES resources.qrc)
endif()

add_executable(kclock main.cpp ${kclock_SRCS} ${RESOURCES})
add_dependencies(kclock kclockd)
target_link_libraries(kclock
        Qt::Qml
        Qt::Gui
        Qt::QuickControls2
        Qt::Widgets
        Qt::Multimedia
        KF${QT_MAJOR_VERSION}::I18n
        KF${QT_MAJOR_VERSION}::ConfigCore
        KF${QT_MAJOR_VERSION}::ConfigGui
        KF${QT_MAJOR_VERSION}::CoreAddons
        KF${QT_MAJOR_VERSION}::Notifications
        KF${QT_MAJOR_VERSION}::DBusAddons
        )
target_include_directories(kclock PRIVATE ${CMAKE_BINARY_DIR})
install(TARGETS kclock ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
