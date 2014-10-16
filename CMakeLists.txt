cmake_minimum_required(VERSION 2.8.12)

project(khangman)

option(BUILD_KHANGMAN_DESKTOP       "Build KHangMan for KDE Desktop"        ON)
option(BUILD_KHANGMAN_HARMATTAN     "Build KHangMan for Harmattan"          OFF)
option(BUILD_KHANGMAN_PLASMA_ACTIVE "Build KHangMan for Plasma Active"      OFF)

include(FeatureSummary)
find_package(Qt5 REQUIRED COMPONENTS Core Qml Quick OpenGL Svg WebKit)
find_package(Qt5 OPTIONAL_COMPONENTS PrintSupport)

find_package(ECM 0.0.9 NO_MODULE REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMInstallIcons)
include(GenerateExportHeader)

find_package(KF5 REQUIRED COMPONENTS
    I18n
    Crash
    Completion
    Config
    CoreAddons
    ConfigWidgets
    DocTools
    KIO
    NewStuff
    Notifications
    XmlGui
    KDE4Support
)

find_package(LibKEduVocDocument REQUIRED)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

find_package(Phonon4Qt5 4.6.60)

add_subdirectory( doc )
add_subdirectory( src )

if(BUILD_KHANGMAN_DESKTOP)
    add_subdirectory( themes )
    add_subdirectory( fonts )
endif()

add_subdirectory( sounds )
add_subdirectory( icons )

install(FILES khangman.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata/)
