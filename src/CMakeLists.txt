set(kclock_SRCS
    alarms.cpp
    timermodel.cpp
    utilmodel.cpp
    resources.qrc
    )

add_executable(kclock main.cpp timezoneselectormodel.cpp ${kclock_SRCS} )
target_link_libraries(kclock
        Qt5::Qml
        Qt5::Gui
        Qt5::QuickControls2
        Qt5::Widgets
        KF5::I18n
        KF5::ConfigCore
        KF5::CoreAddons
        KF5::Notifications
        )

if (ANDROID)
    kirigami_package_breeze_icons(ICONS
        clock
        player-time
        chronometer
        notifications
        settings-configure
        list-add
        notifications
        entry-edit
        delete
        checkmark
        chronometer-reset
        chronometer-lap
        chronometer-pause
        chronometer-start
        globe
    )
endif()
        
install(TARGETS kclock ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
