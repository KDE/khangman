########### install files ###############
#

kdoctools_create_handbook(index.docbook INSTALL_DESTINATION ${HTML_INSTALL_DIR}/en SUBDIR khangman)
kdoctools_create_manpage(man-khangman.6.docbook 6 INSTALL_DESTINATION ${MAN_INSTALL_DIR})
