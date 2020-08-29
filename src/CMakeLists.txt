set(kclock_SRCS
    alarms.cpp
    alarmmodel.cpp
    timermodel.cpp
    utilmodel.cpp
    stopwatchtimer.cpp
    alarmwaitworker.cpp
    alarmplayer.cpp
    kclockformat.cpp
    resources.qrc
    )
kconfig_add_kcfg_files(kclock_SRCS kclocksettings.kcfgc GENERATE_MOC)
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
add_subdirectory(plasmoid)
add_subdirectory(kclockd)
install(TARGETS kclock ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
