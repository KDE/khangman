set(khangman_SRCS
   khangman.cpp
   main.cpp
   khangmanview.cpp
   khmtheme.cpp
   khmthemefactory.cpp
   langutils.cpp
   timer.cpp)


ki18n_wrap_ui(khangman_SRCS generalui.ui languageui.ui timerui.ui )

kconfig_add_kcfg_files(khangman_SRCS prefs.kcfgc )

#kde4_add_app_icon(khangman_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../../icons/hi*-app-khangman.png")

include_directories(
    ${PHONON_INCLUDES}
)

add_executable(khangman ${khangman_SRCS})
add_dependencies(khangman kvtml_data)

target_link_libraries(khangman
    LibKEduVocDocumentImport__KEduVocDocument
    KF5::NewStuff
    KF5::XmlGui
    KF5::I18n
    KF5::Completion
    KF5::Crash
    KF5::Notifications
    Qt5::Gui
    Qt5::Svg
    Phonon::phonon4qt5
)

install(TARGETS khangman  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS org.kde.khangman.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES khangman.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES khangman.knsrc  DESTINATION  ${CONFIG_INSTALL_DIR} )
install( FILES khangmanui.rc  DESTINATION  ${KXMLGUI_INSTALL_DIR}/khangman )
