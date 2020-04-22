qt5_add_resources(kclock_SRCS resources.qrc)

add_executable(kirigamiclock main.cpp timezoneselectormodel.cpp ${kirigamiclock_SRCS} alarms.cpp timermodel.cpp utilmodel.cpp)
target_link_libraries(kclock
        Qt5::Qml
        Qt5::Gui
        Qt5::QuickControls2
        Qt5::Widgets
        Qt5::Multimedia
        KF5::I18n
        KF5::ConfigCore
        KF5::CoreAddons
        KF5::Notifications
        )
        
install(TARGETS kclock ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
