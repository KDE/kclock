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
    stopwatchmodel.cpp
    stopwatchmodel.h
    addlocationmodel.cpp
    addlocationmodel.h
    timerpresetmodel.cpp
    timerpresetmodel.h
    savedlocationsmodel.cpp
    savedlocationsmodel.h
    windowexposure.cpp
    windowexposure.h
    )

# So we can share the enums.
kconfig_add_kcfg_files(kclock_SRCS ../kclockd/kclockdsettings.kcfgc GENERATE_MOC)
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

add_executable(kclock main.cpp ${kclock_SRCS} ${RESOURCES})
add_dependencies(kclock kclockd)

ecm_add_qml_module(kclock URI org.kde.kclock QML_FILES
    qml/Main.qml

    qml/alarm/AlarmForm.qml
    qml/alarm/AlarmFormPage.qml
    qml/alarm/AlarmListPage.qml
    qml/alarm/AlarmListDelegate.qml
    qml/alarm/AlarmRingingPopup.qml
    qml/alarm/SoundPickerPage.qml

    qml/components/AboutPage.qml
    qml/components/BottomToolbar.qml
    qml/components/DialogComboBox.qml
    qml/components/FooterToolBarButton.qml
    qml/components/FloatingActionButton.qml
    qml/components/Sidebar.qml
    qml/components/TimePicker.qml
    qml/components/TimePickerSpinBox.qml
    qml/components/TimePickerSpinBoxButton.qml

    qml/timer/PresetDurationButton.qml
    qml/timer/TimerComponent.qml
    qml/timer/TimerForm.qml
    qml/timer/TimerFormDialog.qml
    qml/timer/TimerListDelegate.qml
    qml/timer/TimerListPage.qml
    qml/timer/TimerPage.qml
    qml/timer/TimerRingingPopup.qml

    qml/settings/SettingsPage.qml

    qml/stopwatch/StopwatchPage.qml

    qml/time/AddLocationListView.qml
    qml/time/AddLocationPage.qml
    qml/time/AddLocationWrapper.qml
    qml/time/AnalogClock.qml
    qml/time/AnalogClockHand.qml
    qml/time/TimePage.qml
    qml/time/TimePageDelegate.qml
)

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
    KF6::Crash
    KF6::I18nQml
    KF6::Svg
)

target_include_directories(kclock PRIVATE ${CMAKE_BINARY_DIR})
install(TARGETS kclock ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
