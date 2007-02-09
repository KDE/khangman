
include_directories( ${CMAKE_SOURCE_DIR}/libkdeedu/kdeeducore  )

########### next target ###############

set(khangman_SRCS 
   khangman.cpp 
   main.cpp 
   khangmanview.cpp 
   khnewstuff.cpp 
   khmtheme.cpp
   langutils.cpp
   timer.cpp )

kde4_automoc(${khangman_SRCS})

kde4_add_ui_files(khangman_SRCS generalui.ui languageui.ui timerui.ui )

kde4_add_kcfg_files(khangman_SRCS prefs.kcfgc )

kde4_add_executable(khangman ${khangman_SRCS})

target_link_libraries(khangman  ${KDE4_KDEUI_LIBS} kdeeducore ${KDE4_KNEWSTUFF_LIBS} ${KDE4_PHONONCORE_LIBRARY} )

install(TARGETS khangman  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES khangman.desktop  DESTINATION  ${XDG_APPS_DIR} )
install( FILES khangman.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES khangmanrc  DESTINATION  ${CONFIG_INSTALL_DIR} )
install( FILES khangmanui.rc  DESTINATION  ${DATA_INSTALL_DIR}/khangman )


