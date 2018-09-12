cmake_minimum_required(VERSION 3.0)

find_package(Qt5 REQUIRED COMPONENTS Gui Qml QuickControls2 Widgets)

set(CMAKE_AUTOMOC ON)

find_package(KF5Kirigami2)
find_package(KF5I18n)
find_package(KF5CoreAddons)
find_package(KF5Config)
add_subdirectory(src)
