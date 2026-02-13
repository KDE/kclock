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

    wayland/pipshellsurface.cpp
    wayland/pipshellsurface.h
    wayland/WaylandAboveLockscreen.cpp
    wayland/WaylandAboveLockscreen.h
    utils/LockscreenUtils.cpp
    utils/LockscreenUtils.h
    pipwindow.cpp
    pipwindow.h
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

set_source_files_properties(qml/components/PipHandler.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

ecm_add_qml_module(kclock URI org.kde.kclock QML_FILES
    qml/Main.qml

    qml/alarm/AlarmForm.qml
    qml/alarm/AlarmFormPage.qml
    qml/alarm/AlarmListPage.qml
    qml/alarm/AlarmListDelegate.qml
    qml/alarm/AlarmRingingPopup.qml
    qml/alarm/AlarmLockscreenPopup.qml

    qml/components/SoundPickerPage.qml
    qml/components/AboutPage.qml
    qml/components/BottomToolbar.qml
    qml/components/DialogComboBox.qml
    qml/components/FooterToolBarButton.qml
    qml/components/FloatingActionButton.qml
    qml/components/PipHandler.qml
    qml/components/Sidebar.qml
    qml/components/TimePicker.qml
    qml/components/TimePickerSpinBox.qml
    qml/components/TimePickerSpinBoxButton.qml
    qml/components/LockscreenAlertView.qml

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

qt6_generate_wayland_protocol_client_sources(kclock
    PRIVATE_CODE
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/wayland/xx-pip-v1.xml
        ${WaylandProtocols_DATADIR}/stable/xdg-shell/xdg-shell.xml
        ${Wayland_DATADIR}/wayland.xml
)


if (KCLOCK_BUILD_SHELL_OVERLAY)
    find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED Gui WaylandClient)

    if (Qt6Gui_VERSION VERSION_GREATER_EQUAL "6.10.0")
        find_package(Qt6 REQUIRED COMPONENTS GuiPrivate WaylandClientPrivate)
    else()
        find_package(Qt6Gui ${QT_MIN_VERSION} CONFIG REQUIRED Private)
        find_package(Qt6WaylandClient ${QT_MIN_VERSION} COMPONENTS CONFIG REQUIRED Private)
    endif()

    find_package(Wayland REQUIRED)
    find_package(PlasmaWaylandProtocols 1.8 CONFIG)
    find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS WindowSystem)
    set_package_properties(PlasmaWaylandProtocols PROPERTIES
        TYPE REQUIRED
        PURPOSE "Collection of Plasma-specific Wayland protocols"
        URL "https://invent.kde.org/libraries/plasma-wayland-protocols/"
    )
    find_package(QtWaylandScanner)
    set_package_properties(QtWaylandScanner PROPERTIES
        TYPE REQUIRED
        PURPOSE "Required for building with Wayland above-lock-screen support"
    )

endif() # KCLOCK_BUILD_SHELL_OVERLAY
target_link_libraries(kclock PRIVATE
    Qt6::Qml
    Qt6::Gui
    Qt6::QuickControls2
    Qt6::WaylandClientPrivate
    Qt6::Widgets
    Qt6::Multimedia
    KF6::I18n
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::CoreAddons
    KF6::DBusAddons
    KF6::Crash
    KF6::I18nQml
    KF6::Svg
)

if(KCLOCK_BUILD_SHELL_OVERLAY)
    
    target_link_libraries(kclock PRIVATE
        Qt6::GuiPrivate
        Qt6::WaylandClient
        Wayland::Client
        KF6::WindowSystem
    )

    if (Qt6_VERSION VERSION_GREATER_EQUAL "6.8.0")
        set(private_code_option  "PRIVATE_CODE")
    endif()

    qt6_generate_wayland_protocol_client_sources(kclock FILES
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/kde-lockscreen-overlay-v1.xml
        ${private_code_option}
    )

endif() # KCLOCK_BUILD_SHELL_OVERLAY
target_include_directories(kclock PRIVATE ${CMAKE_BINARY_DIR})
install(TARGETS kclock ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
