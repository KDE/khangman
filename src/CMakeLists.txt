ecm_setup_version(${KDE_APPLICATIONS_VERSION} VARIABLE_PREFIX KHANGMAN VERSION_HEADER khangman_version.h)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDES}
    ${PHONON_INCLUDES}
)

set(khangman_SRCS main.cpp khangman.cpp khmtheme.cpp khmthemefactory.cpp langutils.cpp)
kconfig_add_kcfg_files(khangman_SRCS prefs.kcfgc)
add_executable(khangman ${khangman_SRCS})
# generate_export_header(khangman BASE_NAME KHANGMAN)

target_link_libraries(khangman
    Qt5::QuickWidgets
    Qt5::Core
    KF5::Crash
    KF5::I18n
    KF5::NewStuff
    KF5::Declarative
    LibKEduVocDocumentImport__KEduVocDocument
)

install(
    FILES
    khangman.kcfg
    DESTINATION
    ${KDE_INSTALL_KCFGDIR}
)

install(
    FILES
    khangman.knsrc
    DESTINATION
    ${KDE_INSTALL_CONFDIR}
)

install(
    DIRECTORY qml/
    DESTINATION ${KDE_INSTALL_DATADIR}/khangman/qml)

install(
    FILES org.kde.khangman.desktop
    DESTINATION ${KDE_INSTALL_APPDIR}
)

install(
    TARGETS khangman  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
)
