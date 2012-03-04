project(khangman)

option(BUILD_KHANGMAN_HARMATTAN "Build KHangMan for Harmattan"      ON)
option(BUILD_KHANGMAN_DESKTOP   "Build KHangMan for KDE Desktop"    ON)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

set(CMAKE_MODULE_PATH ${khangman_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(LibKdeEdu REQUIRED)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)


add_subdirectory( doc )
add_subdirectory( src )

add_subdirectory( themes )
add_subdirectory( sounds )
add_subdirectory( icons )
add_subdirectory( fonts )
