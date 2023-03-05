ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX KHANGMAN VERSION_HEADER khangman_version.h)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDES}
    ${PHONON_INCLUDES}
)

add_executable(khangman)
target_sources(khangman PRIVATE khangman.cpp
    khangman.h
    khmtheme.cpp
    khmthemefactory.cpp
    khmthemefactory.h
    khmtheme.h
    langutils.cpp
    langutils.h
    main.cpp
)
kconfig_add_kcfg_files(khangman prefs.kcfgc)
# generate_export_header(khangman BASE_NAME KHANGMAN)

target_link_libraries(khangman
    Qt${QT_MAJOR_VERSION}::QuickWidgets
    Qt${QT_MAJOR_VERSION}::Core
    KF${KF_MAJOR_VERSION}::Crash
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::NewStuffWidgets
    KF${KF_MAJOR_VERSION}::ConfigCore
    KF${KF_MAJOR_VERSION}::WidgetsAddons
    KF${KF_MAJOR_VERSION}::XmlGui
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
