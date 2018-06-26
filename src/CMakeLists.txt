
qt5_add_resources(kirigamiclock_SRCS resources.qrc)
add_executable(kirigamiclock main.cpp ${kirigamiclock_SRCS})
target_link_libraries(kirigamiclock Qt5::Qml Qt5::Gui Qt5::QuickControls2)
target_link_libraries(kirigamiclock KF5::Kirigami2)
target_link_libraries(kirigamiclock KF5::I18n)
