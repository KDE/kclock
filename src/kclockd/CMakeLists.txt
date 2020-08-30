set(kclockd_SRCS
    alarms.cpp
    alarmmodel.cpp
    alarmwaitworker.cpp
    alarmplayer.cpp
    )
# generate KConfig class from xml
kconfig_add_kcfg_files(kclockd_SRCS kclockdsettings.kcfgc GENERATE_MOC)

# use generated KConfig header file to generate DBus adaptor xml
set(kclockd_SRCS
    ${kclockd_SRCS}
    ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.xml
)
qt5_generate_dbus_interface(
    ${CMAKE_CURRENT_BINARY_DIR}/kclockdsettings.h
    org.kde.kclockd.xml
    OPTIONS -S -P
)

# use generated DBus adapator xml to generate adaptor source code
qt5_add_dbus_adaptor(kclockd_SRCS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.xml
                     ${CMAKE_CURRENT_BINARY_DIR}/kclockdsettings.h KClockSettings)

# install DBus interface xml to dbus interface directory
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kclockd.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})

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
