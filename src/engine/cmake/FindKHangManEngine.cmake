# - Try to find the KHangMan Engine library.
#
# Once done this will define
#  KHANGMAN_ENGINE_FOUND - The KHangMan Engine library was found
#  KHANGMAN_ENGINE_INCLUDE_DIR - the KHangMan Engine include directory
#  KHANGMAN_ENGINE_INCLUDE_DIRS - all include directories required for the
#  KHANGMAN_ENGINE_LIBRARY - The KHangMan Engine library location
#  KHANGMAN_ENGINE_LIBRARIES - The libraries to link against to use KHangMan Engine
#
# Copyright (C) 2012 Laszlo Papp
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the COPYING-CMAKE-SCRIPTS file from Gluon's Source tree.

if (KHANGMAN_ENGINE_LIBRARY AND KHANGMAN_ENGINE_INCLUDE_DIR)
  # Already in cache, be silent
  set(KHANGMAN_ENGINE_FOUND TRUE)
endif (KHANGMAN_ENGINE_LIBRARY AND KHANGMAN_ENGINE_INCLUDE_DIR)

if (KHangManEngine_FIND_REQUIRED)
    set(_khangmanEngineReq "REQUIRED")
endif (KHangManEngine_FIND_REQUIRED)

find_path(KHANGMAN_INSTALL_PREFIX
    NAMES
    include/khangman
)

find_package(LibKdeEdu ${_khangmanEngineReq})

if(NOT LIB_SUFFIX)
    set(_Init_LIB_SUFFIX "")
    if ("${QT_QTCORE_LIBRARY}" MATCHES lib64)
        set(_Init_LIB_SUFFIX 64)
    endif ("${QT_QTCORE_LIBRARY}" MATCHES lib64)
    if ("${QT_QTCORE_LIBRARY}" MATCHES lib32)
        set(_Init_LIB_SUFFIX 32)
    endif ("${QT_QTCORE_LIBRARY}" MATCHES lib32)

    set(LIB_SUFFIX              ${_Init_LIB_SUFFIX} CACHE STRING "The suffix of the system-wide library path")
endif(NOT LIB_SUFFIX)

if(NOT INCLUDE_INSTALL_DIR)
    set(INCLUDE_INSTALL_DIR ${KHANGMAN_INSTALL_PREFIX}/include CACHE PATH "The subdirectory relative to the install prefix where header files will be installed.")
endif()
if(NOT LIB_INSTALL_DIR)
    set(LIB_INSTALL_DIR ${KHANGMAN_INSTALL_PREFIX}/lib${LIB_SUFFIX} CACHE PATH "The subdirectory relative to the install prefix where libraries will be installed.")
endif()
if(NOT SHARE_INSTALL_DIR)
    set(SHARE_INSTALL_DIR ${KHANGMAN_INSTALL_PREFIX}/share CACHE PATH "The subdiractory relative to the install prefix where data and other files will be installed.")
endif()

find_path(KHANGMAN_ENGINE_INCLUDE_DIR
    NAMES
    khangman/khangmanengine.h
    PATHS
    ${INCLUDE_INSTALL_DIR}
    PATH_SUFFIXES
    khangman
)

set(KHANGMAN_ENGINE_INCLUDE_DIRS
    ${KHANGMAN_ENGINE_INCLUDE_DIR}
    ${QT_INCLUDES}
    ${KDE4_INCLUDES}
)

find_library(KHANGMAN_ENGINE_LIBRARY
    NAMES
    khangmanengine
    PATHS
    ${LIB_INSTALL_DIR}
)

set(KHANGMAN_ENGINE_LIBRARIES
    ${QT_QTCORE_LIBRARY}
    ${KDE4_KDECORE_LIBS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(KHangManEngine DEFAULT_MSG KHANGMAN_ENGINE_LIBRARY KHANGMAN_ENGINE_INCLUDE_DIR)
