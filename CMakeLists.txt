#
# Copyright 2020 Han Young <hanyoung@protonmail.com>
# Copyright 2020-2021 Devin Lin <devin@kde.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)

# KDE Applications version, managed by release script.
set(RELEASE_SERVICE_VERSION_MAJOR "24")
set(RELEASE_SERVICE_VERSION_MINOR "02")
set(RELEASE_SERVICE_VERSION_MICRO "2")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(kclock VERSION ${RELEASE_SERVICE_VERSION})

set(KF_MIN_VERSION "5.240.0")
set(QT_MIN_VERSION "6.5.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

# only enable QML debugging on debug builds
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DQT_QML_DEBUG ")

include(FeatureSummary)
include(ECMSetupVersion)
include(ECMFindQmlModule)
include(ECMConfiguredInstall)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(KDEGitCommitHooks)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX KCLOCK
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Gui
    Quick
    Test
    Svg
    QuickControls2
    Multimedia
    DBus
    Widgets
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Config
    I18n
    CoreAddons
    Kirigami
    Notifications
    DBusAddons
    StatusNotifierItem
)
find_package(Plasma REQUIRED)

find_package(KF6KirigamiAddons 0.6 REQUIRED)

ki18n_install(po)

add_subdirectory(src)

install(PROGRAMS org.kde.kclock.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.kclockd-autostart.desktop DESTINATION ${KDE_INSTALL_AUTOSTARTDIR})
install(FILES org.kde.kclock.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.kclock.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps/)
install(FILES kclockd.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})

ecm_install_configured_files(INPUT org.kde.kclockd.service.in DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR} @ONLY)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.hpp)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
