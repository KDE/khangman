cmake_minimum_required(VERSION 2.8)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

####### KHangMan Harmattan ########
####### KHangMan Desktop ########
if(BUILD_KHANGMAN_DESKTOP)
    find_package(KDE4)
    if(KDE4_FOUND)
        include_directories(${KDE4_INCLUDES})
        add_subdirectory(desktop)
    else(KDE4_FOUND)
        message(STATUS "KDE libraries are not found - not building the KDE Desktop frontend")
    endif(KDE4_FOUND)
endif()

if(BUILD_KHANGMAN_HARMATTAN)
    if(QT_QTDECLARATIVE_FOUND)
        add_subdirectory(harmattan)
    else(QT_QTDECLARATIVE_FOUND)
        message(WARNING "Qt installation lacks Qt Declarative - disabling harmattan based frontend")
    endif(QT_QTDECLARATIVE_FOUND)
endif()

if(BUILD_KHANGMAN_PLASMA_ACTIVE)
    find_package(KDE4)
    if(KDE4_FOUND)
        include_directories(${KDE4_INCLUDES})
        add_subdirectory(plasma-active)
    else(KDE4_FOUND)
        message(STATUS "KDE libraries are not found - disable the Plasma Active frontend")
    endif(KDE4_FOUND)
endif()
