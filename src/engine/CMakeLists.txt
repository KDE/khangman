set(CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

set(KHANGMANENGINE_LIB_VERSION 5.0.0)
set(KHANGMANENGINE_LIB_SOVERSION 5)

set(INCLUDE_INSTALL_DIR
    include
    CACHE INTERNAL "The subdirectory relative to the install prefix where header files will be installed."
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
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

kconfig_add_kcfg_files(KHangManEngine_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/prefs.kcfgc)

if(APPLE)
    add_library(khangmanengine SHARED ${KHangManEngine_SRCS} ${KHangManEngine_HEADERS})
else()
    add_library(khangmanengine SHARED ${KHangManEngine_SRCS})
endif()

target_link_libraries(
    khangmanengine
    Qt5::Widgets
    KF5::CoreAddons
    KF5::I18n
    LibKEduVocDocumentImport__KEduVocDocument
)

set(KHANGMAN_ENGINE_LIBRARIES
    khangmanengine
    ${LIBKEDUVOCDOCUMENT_LIBRARIES}
    CACHE INTERNAL "Libraries required for KHangMan engine"
)

set_target_properties(khangmanengine PROPERTIES VERSION ${KHANGMANENGINE_LIB_VERSION} SOVERSION ${KHANGMANENGINE_LIB_SOVERSION} DEFINE_SYMBOL MAKE_KHANGMAN_ENGINE_LIB)

generate_export_header(khangmanengine BASE_NAME KHANGMAN_ENGINE)

if(APPLE)
    #hack for being able to set headers as public in a osx framework
    list(APPEND KHangManEngine_HEADERS
        ${KHangManEngine_HEADERS}
    )

    set_target_properties(khangmanengine PROPERTIES FRAMEWORK TRUE)
    set_target_properties(khangmanengine PROPERTIES BUILD_WITH_INSTALL_RPATH 1 INSTALL_NAME_DIR "@executable_path/../Frameworks")
    set_target_properties(khangmanengine PROPERTIES PUBLIC_HEADER "${KHangManEngine_HEADERS}")

    set(MACOSX_FRAMEWORK_IDENTIFIER engine)
    set(MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${KHANGMANENGINE_LIB_SOVERSION})
    set(MACOSX_FRAMEWORK_BUNDLE_VERSION ${KHANGMANENGINE_LIB_VERSION})
endif()

install(
    TARGETS     khangmanengine
    RUNTIME     DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT khangmanengine
    LIBRARY     DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT khangmanengine
    ARCHIVE     DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT khangmanengine
    FRAMEWORK   DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT khangmanengine
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

install(
    FILES
    khangman.kcfg
    DESTINATION
    ${KCFG_INSTALL_DIR}
)

install(
    FILES
    khangman.knsrc
    DESTINATION
    ${CONFIG_INSTALL_DIR}
)

