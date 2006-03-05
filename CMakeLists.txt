kde4_header()

add_subdirectory( khangman ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############


kde4_create_doxygen_docs(  )

kde4_footer()



#original Makefile.am contents follow:

######## kdevelop will overwrite this part!!! (begin)##########
#
#SUBDIRS = khangman 
#
#EXTRA_DIST = TODO README AUTHORS khangman.lsm INSTALL ChangeLog COPYING khangman.kdevprj 
#
######## kdevelop will overwrite this part!!! (end)############
## not a GNU package. You can remove this line, if 
## have all needed files, that a GNU package needs
#AUTOMAKE_OPTIONS = foreign 
#
#include ../admin/Doxyfile.am
#
