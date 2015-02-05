include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDES}
    ${LIBKEDUVOCDOCUMENT_INCLUDE_DIR}
    ${PHONON_INCLUDES}
)

set(khangman_SRCS main.cpp khangman.cpp khmtheme.cpp khmthemefactory.cpp langutils.cpp)
kconfig_add_kcfg_files(khangman_SRCS prefs.kcfgc)
add_executable(khangman ${khangman_SRCS})
# generate_export_header(khangman BASE_NAME KHANGMAN)

target_link_libraries(khangman
    Qt5::QuickWidgets
    Qt5::Core
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
    DIRECTORY qml/
    DESTINATION ${DATA_INSTALL_DIR}/khangman/qml)

install(
    FILES org.kde.khangman.desktop
    DESTINATION ${XDG_APPS_INSTALL_DIR}
)

install(
    TARGETS khangman  ${INSTALL_TARGETS_DEFAULT_ARGS}
)
