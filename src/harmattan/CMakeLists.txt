cmake_minimum_required(VERSION 2.8)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}

    ${KHANGMAN_ENGINE_INCLUDE_DIRS}
)

set(khangman-harmattan_SRCS
    main.cpp
    khangmanenginehelper.cpp
)

# generate rules for building source files from the resources
QT4_ADD_RESOURCES(KHANGMAN-HARMATTAN_RCC_SRCS
    khangman-harmattan.qrc
)

kde4_add_executable(khangman-harmattan ${khangman-harmattan_SRCS} ${KHANGMAN-HARMATTAN_RCC_SRCS})

target_link_libraries(khangman-harmattan
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    ${KHANGMAN_ENGINE_LIBRARIES}
    ${KDE4_KDECORE_LIBS}
    kdeclarative
)

install(TARGETS khangman-harmattan
        DESTINATION ${BIN_INSTALL_DIR}
        COMPONENT khangmanharmattan
)

install(FILES khangman.sh
        DESTINATION ${BIN_INSTALL_DIR}
        COMPONENT khangmanharmattan
)

install(FILES khangman-harmattan.desktop
        DESTINATION ${SHARE_INSTALL_DIR}/applications
        COMPONENT khangmanharmattan)

install(FILES khangman.conf
        DESTINATION ${SHARE_INSTALL_DIR}/policy/etc/pulse/xpolicy.conf.d
        COMPONENT khangmanharmattan)

if(NOT HANGMAN_ICONS_INSTALLED)
    add_subdirectory(../../icons icons)
    set(KHANGMAN_ICONS_INSTALLED TRUE)
endif()
