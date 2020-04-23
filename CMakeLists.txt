project(kclock)

cmake_minimum_required(VERSION 3.0)
set(KF5_MIN_VERSION "5.18.0")
set(QT_MIN_VERSION "5.7.0")

################# Disallow in-source build #################

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   message(FATAL_ERROR "This application requires an out of source build. Please create a separate build directory.")
endif()

include(FeatureSummary)

################# set KDE specific information #################

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(ECMPoQmTools)
include(KDECompilerSettings NO_POLICY_SCOPE)

# kirigami-addons (not a package yet, so this is a runtime dependency)
include(ECMQMLModules)
ecm_find_qmlmodule(org.kde.kirigamiaddons.dateandtime 0.1)

################# Find dependencies #################

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS DBus Core Quick Test Gui Svg QuickControls2 Test)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Config
    Kirigami2
    I18n
    CoreAddons
    Config
    Notifications
)

################ Setup notifications ################

install(FILES kclock.notifyrc DESTINATION ${KNOTIFYRC_INSTALL_DIR})

################# build and install #################
add_subdirectory(src)

install(PROGRAMS org.kde.kclock.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.kclock.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
