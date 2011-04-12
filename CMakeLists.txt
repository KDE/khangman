project(khangman)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

set(CMAKE_MODULE_PATH ${khangman_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

add_subdirectory( doc )
add_subdirectory( src )

add_subdirectory( themes )
add_subdirectory( sounds )
add_subdirectory( icons )
add_subdirectory( fonts )
