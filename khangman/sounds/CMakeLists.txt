kde4_header()




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/khangman/sounds FILES new_game.ogg EW_Dialogue_Appear.ogg )

kde4_footer()



#original Makefile.am contents follow:

#soundsdir = $(kde_datadir)/khangman/sounds
#sounds_DATA = new_game.ogg EW_Dialogue_Appear.ogg
#EXTRA_DIST = $(sounds_DATA)
