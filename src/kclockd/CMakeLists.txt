set(kclockd_SRCS
    alarms.cpp
    alarmmodel.cpp
    alarmwaitworker.cpp
    alarmplayer.cpp
    )
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
install(TARGETS kclockd ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
