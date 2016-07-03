cmake_minimum_required(VERSION 2.8.12)

project(khangman)

option(BUILD_KHANGMAN_DESKTOP       "Build KHangMan for KDE Desktop"        ON)
option(BUILD_KHANGMAN_HARMATTAN     "Build KHangMan for Harmattan"          OFF)
option(BUILD_KHANGMAN_PLASMA_ACTIVE "Build KHangMan for Plasma Active"      OFF)

include(FeatureSummary)
find_package(Qt5 REQUIRED COMPONENTS Core Qml Quick QuickWidgets Svg)

find_package(ECM 1.7.0 NO_MODULE REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(GenerateExportHeader)
include(ECMAddAppIcon)

find_package(KF5 5.15.0 REQUIRED COMPONENTS
    I18n
    Crash
    Completion
    Config
    CoreAddons
    ConfigWidgets
    Declarative
    DocTools
    NewStuff
    Notifications
    XmlGui
    KIO
)

find_package(LibKEduVocDocument REQUIRED)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

add_subdirectory( doc )
add_subdirectory( src )

if(BUILD_KHANGMAN_DESKTOP)
    add_subdirectory( themes )
    add_subdirectory( fonts )
endif()

add_subdirectory( icons )

install(FILES org.kde.khangman.appdata.xml DESTINATION ${KDE_INSTALL_DATAROOTDIR}/appdata/)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
