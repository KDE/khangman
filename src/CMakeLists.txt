
include_directories( ${CMAKE_SOURCE_DIR}/libkdeedu/keduvocdocument  )

########### next target ###############

set(khangman_SRCS 
   khangman.cpp 
   main.cpp 
   khangmanview.cpp 
   khmtheme.cpp
   langutils.cpp
   timer.cpp 
   fontchecker.cpp)


kde4_add_ui_files(khangman_SRCS generalui.ui languageui.ui timerui.ui )

kde4_add_kcfg_files(khangman_SRCS prefs.kcfgc )

if(MSVC)
  set(khangman_SRCS ${khangman_SRCS} khangman.rc)
endif(MSVC)

if(MINGW)
   KDE_ADD_RESOURCES2(khangman_SRCS khangman.rc)
endif(MINGW)

kde4_add_executable(khangman ${khangman_SRCS})

target_link_libraries(khangman  keduvocdocument ${KDE4_KNEWSTUFF2_LIBS} ${KDE4_PHONON_LIBS} )

install(TARGETS khangman  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES khangman.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES khangman.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES khangman.knsrc  DESTINATION  ${CONFIG_INSTALL_DIR} )
install( FILES khangmanui.rc  DESTINATION  ${DATA_INSTALL_DIR}/khangman )


