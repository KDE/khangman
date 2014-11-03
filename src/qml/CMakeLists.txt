include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDES}
    ${LIBKEDUVOCDOCUMENT_INCLUDE_DIR}
    ${PHONON_INCLUDES}
)

#kconfig_add_kcfg_files(khangman_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/prefs.kcfgc)
# qt5_add_dbus_interfaces(kanagram_SRCS ${KDE4_DBUS_INTERFACES_DIR}/org.kde.KSpeech.xml )
add_executable(khangman ${khangman_SRCS})

target_link_libraries(khangman
    Qt5::Quick
    Qt5::Core
    Qt5::WebKit
    KF5::I18n
    KF5::NewStuff
    KF5::SonnetCore
    KF5::Declarative
    LibKEduVocDocumentImport__KEduVocDocument
    Phonon::phonon4qt5
)

install(
    FILES
    khangman.kcfg
    DESTINATION
    ${KCFG_INSTALL_DIR}
)

install(
    FILES
    khangman.knsrc
    DESTINATION
    ${CONFIG_INSTALL_DIR}
)

install(
    DIRECTORY ui/
    DESTINATION ${DATA_INSTALL_DIR}/khangman/ui)

install(
    FILES khangman.desktop
    DESTINATION ${XDG_APPS_INSTALL_DIR}
)