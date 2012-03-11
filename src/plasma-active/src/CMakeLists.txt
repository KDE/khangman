include_directories(
    ${KDE4_INCLUDES}
    ${QT_INCLUDES}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(khangman-active_SRCS
    kdeclarativemainwindow.cpp
    kdeclarativeview.cpp
    main.cpp
    mainwindow.cpp
)

kde4_add_executable(khangman-active ${khangman-active_SRCS})

target_link_libraries(khangman-active
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTOPENGL_LIBRARY}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_PLASMA_LIBS}
    kdeclarative
)

install(
    TARGETS
    khangman-active
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)
