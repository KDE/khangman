cmake_minimum_required(VERSION 3.16)

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "22")
set(RELEASE_SERVICE_VERSION_MINOR "03")
set(RELEASE_SERVICE_VERSION_MICRO "70")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

set(QT_MIN_VERSION "5.15.0")
set(KF5_MIN_VERSION "5.82.0")

project(khangman VERSION ${RELEASE_SERVICE_VERSION})

include(FeatureSummary)
find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Qml Quick QuickWidgets Svg)

find_package(ECM ${KF5_MIN_VERSION} NO_MODULE REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} )

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(GenerateExportHeader)
include(ECMAddAppIcon)
include(ECMSetupVersion)

find_package(KF5 5.81.0 REQUIRED COMPONENTS
    I18n
    Crash
    Completion
    Config
    CoreAddons
    ConfigWidgets
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

add_subdirectory( themes )
add_subdirectory( fonts )

add_subdirectory( icons )

ki18n_install(po)
if (KF5DocTools_FOUND)
    kdoctools_install(po)
endif()
install(FILES org.kde.khangman.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
