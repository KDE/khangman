FILE(GLOB datafiles *.kvtml)

install( FILES ${datafiles}  DESTINATION  ${DATA_INSTALL_DIR}/kvtml/en )
