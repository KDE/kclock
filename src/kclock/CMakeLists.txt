set(kclock_SRCS
    alarms.cpp
    alarmmodel.cpp
    timermodel.cpp
    utilmodel.cpp
    stopwatchtimer.cpp
    alarmplayer.cpp
    kclockformat.cpp
    kclock_algorithm.hpp
    resources.qrc
    )

set(DBusXML ${CMAKE_CURRENT_BINARY_DIR}/../org.kde.kclockd.KClockSettings.xml)
set(AlarmXML ${CMAKE_CURRENT_BINARY_DIR}/../org.kde.kclockd.Alarm.xml)
set(AlarmModelXML ${CMAKE_CURRENT_BINARY_DIR}/../org.kde.kclockd.AlarmModel.xml)

qt5_add_dbus_interface(kclock_SRCS ${DBusXML} kclocksettingsinterface )
qt5_add_dbus_interface(kclock_SRCS ${AlarmXML} alarminterface )
qt5_add_dbus_interface(kclock_SRCS ${AlarmModelXML} alarmmodelinterface )

add_executable(kclock main.cpp timezoneselectormodel.cpp ${kclock_SRCS} )
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
install(TARGETS kclock ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
