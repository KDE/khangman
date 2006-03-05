kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/khangman/data/en FILES easy.kvtml animals.kvtml hard.kvtml medium.kvtml )

kde4_footer()



#original Makefile.am contents follow:

#txt_DATA = easy.kvtml animals.kvtml hard.kvtml medium.kvtml
#txtdir = $(kde_datadir)/khangman/data/en
#EXTRA_DIST = $(txt_DATA)
