qt5_add_resources(kirigamiclock_SRCS resources.qrc)

add_executable(kirigamiclock main.cpp timezoneselectormodel.cpp ${kirigamiclock_SRCS} alarms.cpp timermodel.cpp timermodel.h)
target_link_libraries(kirigamiclock
        Qt5::Qml
        Qt5::Gui
        Qt5::QuickControls2
        Qt5::Widgets
        KF5::I18n
        KF5::ConfigCore
        KF5::CoreAddons
        )

install(TARGETS kirigamiclock ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
