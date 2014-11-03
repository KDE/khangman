include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDES}
    ${LIBKEDUVOCDOCUMENT_INCLUDE_DIR}
    ${PHONON_INCLUDES}
)

set(khangman_SRCS main.cpp khangman.cpp khmtheme.cpp khmthemefactory.cpp langutils.cpp timer.cpp khangmanengine.cpp khangmanenginehelper.cpp)
qt5_wrap_ui(khangman_SRCS generalui.ui languageui.ui timerui.ui)
kconfig_add_kcfg_files(khangman_SRCS prefs.kcfgc)
add_executable(khangman ${khangman_SRCS})
# generate_export_header(khangman BASE_NAME KHANGMAN)

target_link_libraries(khangman
    Qt5::QuickWidgets
    Qt5::Core
    Qt5::WebKit
    KF5::I18n
    KF5::NewStuff
    KF5::SonnetCore
    KF5::Declarative
    LibKEduVocDocumentImport__KEduVocDocument
    Phonon::phonon4qt5
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

install(
    DIRECTORY qml/
    DESTINATION ${DATA_INSTALL_DIR}/khangman/qml)

install(
    FILES khangman.desktop
    DESTINATION ${XDG_APPS_INSTALL_DIR}
)

install(
    TARGETS khangman  ${INSTALL_TARGETS_DEFAULT_ARGS}
)

####### KHangMan Harmattan ########
if(BUILD_KHANGMAN_HARMATTAN)
    if(QT_QTDECLARATIVE_FOUND)
        add_subdirectory(harmattan)
    else(QT_QTDECLARATIVE_FOUND)
        message(WARNING "Qt installation lacks Qt Declarative - disabling harmattan based frontend")
    endif(QT_QTDECLARATIVE_FOUND)
endif()

####### KHangMan Plasma Active ########
if(BUILD_KHANGMAN_PLASMA_ACTIVE)
    find_package(KDE4)
    if(KDE4_FOUND)
        include_directories(${KDE4_INCLUDES})
        add_subdirectory(plasma-active)
    else(KDE4_FOUND)
        message(STATUS "KDE libraries are not found - disable the Plasma Active frontend")
    endif(KDE4_FOUND)
endif()
