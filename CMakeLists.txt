cmake_minimum_required(VERSION 3.0)

project(kclock)

set(KF5_MIN_VERSION "5.79.0")
set(QT_MIN_VERSION "5.15.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(FeatureSummary)
include(ECMSetupVersion)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(KDEGitCommitHooks)
include(ECMQMLModules)
include(ECMConfiguredInstall)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Gui
    Quick
    Test
    Svg
    QuickControls2
    Multimedia
    DBus
)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Config
    I18n
    CoreAddons
    Plasma
    Kirigami2
    Notifications
    DBusAddons
)

ecm_find_qmlmodule(org.kde.kirigamiaddons.dateandtime 0.1)

add_subdirectory(src)

install(PROGRAMS org.kde.kclock.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(PROGRAMS org.kde.kclockd-autostart.desktop DESTINATION ${KDE_INSTALL_AUTOSTARTDIR})
install(FILES org.kde.kclock.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES kclock.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps/)
install(FILES kclockd.notifyrc DESTINATION ${KNOTIFYRC_INSTALL_DIR})

ecm_install_configured_files(INPUT org.kde.kclockd.service.in DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR} @ONLY)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
