kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/khangman/sounds FILES new_game.ogg EW_Dialogue_Appear.ogg )

kde4_footer()



#original Makefile.am contents follow:

#soundsdir = $(kde_datadir)/khangman/sounds
#sounds_DATA = new_game.ogg EW_Dialogue_Appear.ogg
#EXTRA_DIST = $(sounds_DATA)
