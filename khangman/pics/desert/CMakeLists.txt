kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/khangman/pics/desert FILES desert_theme.png animation0.png animation1.png animation2.png animation3.png animation4.png animation5.png animation6.png animation7.png animation8.png animation9.png animation10.png  )

kde4_footer()



#original Makefile.am contents follow:

#picsdir = $(kde_datadir)/khangman/pics/desert
#pics_DATA = desert_theme.png animation0.png animation1.png animation2.png animation3.png animation4.png animation5.png animation6.png animation7.png animation8.png animation9.png animation10.png 
#EXTRA_DIST = $(pics_DATA)
#
