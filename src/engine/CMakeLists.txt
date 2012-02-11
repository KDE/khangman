cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

find_package(Qt4 4.7 REQUIRED)

if ("${QT_QTCORE_LIBRARY}" MATCHES lib64)
   set(_Init_LIB_SUFFIX 64)
elseif ("${QT_QTCORE_LIBRARY}" MATCHES lib32)
   set(_Init_LIB_SUFFIX 32)
endif ("${QT_QTCORE_LIBRARY}" MATCHES lib64)

set(LIB_SUFFIX
    ${_Init_LIB_SUFFIX}
    CACHE INTERNAL
    "The suffix of the system-wide library path"
)

set(BIN_INSTALL_DIR
    bin
    CACHE INTERNAL "The subdirectory relative to the install prefix where binary files will be installed."
    )

set(INCLUDE_INSTALL_DIR
    include
    CACHE INTERNAL "The subdirectory relative to the install prefix where header files will be installed."
    )

set(LIB_INSTALL_DIR
    lib${LIB_SUFFIX}
    CACHE INTERNAL "The subdirectory relative to the install prefix where libraries will be installed."
)

set(SHARE_INSTALL_DIR
    share
    CACHE INTERNAL "The subdiractory relative to the install prefix where data and other files will be installed."
)

set(KHANGMAN_ENGINE_INCLUDE_DIRS
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LIBKDEEDU_INCLUDE_DIR}
    CACHE INTERNAL "Includes required for KHangMan Engine"
)

set(KHangManEngine_SRCS
    khangmanengine.cpp
)

set(KHangManEngine_HEADERS
    khangmanengine.h
    khangman_engine_export.h
)

if(BUILD_KHANGMAN_HARMATTAN)
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/harmattan
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/harmattan

        ${LIBKDEEDU_INCLUDE_DIR}
    )

kde4_add_kcfg_files(KHangManEngine_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/harmattan/prefs.kcfgc)
endif()

if(BUILD_KHANGMAN_DESKTOP)
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/desktop
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/desktop

        ${LIBKDEEDU_INCLUDE_DIR}
    )

kde4_add_kcfg_files(KHangManEngine_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/desktop/prefs.kcfgc)
endif()


if(APPLE)
    kde4_add_library(khangmanengine SHARED ${KHangManEngine_SRCS} ${KHangManEngine_HEADERS})
else()
    kde4_add_library(khangmanengine SHARED ${KHangManEngine_SRCS})
endif()

if(BUILD_KHANGMAN_HARMATTAN)
    target_link_libraries(
        khangmanengine
        #${QT_QTXML_LIBRARY}
        ${KDE4_KDECORE_LIBS}
        ${LIBKDEEDU_KEDUVOCDOCUMENT_LIBRARIES}
    )
endif()

if(BUILD_KHANGMAN_DESKTOP)
    target_link_libraries(
        khangmanengine
        #${QT_QTXML_LIBRARY}
        ${KDE4_KDEUI_LIBS}
        ${LIBKDEEDU_KEDUVOCDOCUMENT_LIBRARIES}
    )
endif()

set(KHANGMAN_ENGINE_LIBRARIES
    khangmanengine
    ${LIBKDEEDU_KEDUVOCDOCUMENT_LIBRARIES}
    CACHE INTERNAL "Libraries required for KHangMan engine"
)

set_target_properties(khangmanengine PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} DEFINE_SYMBOL MAKE_KHANGMAN_ENGINE_LIB)

if(APPLE)
    #hack for being able to set headers as public in a osx framework
    list(APPEND KHangManEngine_HEADERS
        ${KHangManEngine_HEADERS}
    )

set_target_properties(KHangManEngine PROPERTIES FRAMEWORK TRUE)
set_target_properties(KHangManEngine PROPERTIES BUILD_WITH_INSTALL_RPATH 1 INSTALL_NAME_DIR "@executable_path/../Frameworks")
set_target_properties(KHangManEngine PROPERTIES PUBLIC_HEADER "${KHangManEngine_HEADERS}")

    set(MACOSX_FRAMEWORK_IDENTIFIER engine)
    set(MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${GENERIC_LIB_SOVERSION})
    set(MACOSX_FRAMEWORK_BUNDLE_VERSION ${GENERIC_LIB_VERSION})
endif()

install(
    TARGETS     khangmanengine
    RUNTIME     DESTINATION ${BIN_INSTALL_DIR} COMPONENT khangmanengine
    LIBRARY     DESTINATION ${LIB_INSTALL_DIR} COMPONENT khangmanengine
    ARCHIVE     DESTINATION ${LIB_INSTALL_DIR} COMPONENT khangmanengine
    FRAMEWORK   DESTINATION ${LIB_INSTALL_DIR} COMPONENT khangmanengine
)

install(
    FILES
    ${KHangManEngine_HEADERS}
    DESTINATION ${INCLUDE_INSTALL_DIR}/khangman
    COMPONENT khangmanengine
)

install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/prefs.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/khangman
    COMPONENT khangmanengine
)

if(BUILD_KHANGMAN_HARMATTAN)
    install(
        FILES
        harmattan/khangman.kcfg
        DESTINATION
        ${KCFG_INSTALL_DIR}
    )
endif()

if(BUILD_KHANGMAN_DESKTOP)
    install(
        FILES
        desktop/khangman.kcfg
        DESTINATION
        ${KCFG_INSTALL_DIR}
    )
endif()

install(
    FILES
    khangman.knsrc
    DESTINATION
    ${CONFIG_INSTALL_DIR}
)

