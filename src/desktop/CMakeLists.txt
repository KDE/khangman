include_directories( ${LIBKDEEDU_INCLUDE_DIR} )

########### next target ###############

set(khangman_SRCS
   khangman.cpp
   main.cpp
   khangmanview.cpp
   khmtheme.cpp
   khmthemefactory.cpp
   langutils.cpp
   timer.cpp)


kde4_add_ui_files(khangman_SRCS generalui.ui languageui.ui timerui.ui )

kde4_add_kcfg_files(khangman_SRCS prefs.kcfgc )

#kde4_add_app_icon(khangman_SRCS "${KDE4_ICON_DIR}/oxygen/*/apps/khangman.png")
#kde4_add_app_icon(khangman_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../icons/ox*-apps-khangman.png")
kde4_add_app_icon(khangman_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../icons/hi*-app-khangman.png")

kde4_add_executable(khangman ${khangman_SRCS})
add_dependencies(khangman kvtml_data)

target_link_libraries(khangman  ${LIBKDEEDU_KEDUVOCDOCUMENT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KNEWSTUFF3_LIBS} ${KDE4_PHONON_LIBS} )

install(TARGETS khangman  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS khangman.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES khangman.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES khangman.knsrc  DESTINATION  ${CONFIG_INSTALL_DIR} )
install( FILES khangmanui.rc  DESTINATION  ${DATA_INSTALL_DIR}/khangman )
