set(kclock_SRCS
    alarm.cpp
    alarmmodel.cpp
    timer.cpp
    timermodel.cpp
    utilmodel.cpp
    stopwatchtimer.cpp
    alarmplayer.cpp
    kclockformat.cpp
    settingsmodel.cpp
    timezoneselectormodel.cpp
    resources.qrc
    timerpresetmodel.cpp
    )

set(SettingsXML ${CMAKE_CURRENT_BINARY_DIR}/../kclockd/org.kde.kclockd.KClockSettings.xml)
set(AlarmXML ${CMAKE_CURRENT_BINARY_DIR}/../kclockd/org.kde.kclockd.Alarm.xml)
set(AlarmModelXML ${CMAKE_CURRENT_BINARY_DIR}/../kclockd/org.kde.kclockd.AlarmModel.xml)
set(TimermModelXML ${CMAKE_CURRENT_BINARY_DIR}/../kclockd/org.kde.kclockd.TimerModel.xml)
set(TimerXML ${CMAKE_CURRENT_BINARY_DIR}/../kclockd/org.kde.kclockd.Timer.xml)

qt5_add_dbus_interface(kclock_SRCS ${SettingsXML} kclocksettingsinterface )
qt5_add_dbus_interface(kclock_SRCS ${AlarmXML} alarminterface )
qt5_add_dbus_interface(kclock_SRCS ${AlarmModelXML} alarmmodelinterface )
qt5_add_dbus_interface(kclock_SRCS ${TimermModelXML} timermodelinterface )
qt5_add_dbus_interface(kclock_SRCS ${TimerXML} timerinterface )

add_executable(kclock main.cpp ${kclock_SRCS} )
add_dependencies(kclock kclockd)
target_link_libraries(kclock
        Qt5::Qml
        Qt5::Gui
        Qt5::QuickControls2
        Qt5::Widgets
        Qt5::Multimedia
        KF5::I18n
        KF5::ConfigCore
        KF5::ConfigGui
        KF5::CoreAddons
        KF5::Notifications
        KF5::DBusAddons
        )
target_include_directories(kclock PRIVATE ${CMAKE_BINARY_DIR})
install(TARGETS kclock ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
