# SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

qt_add_qml_module(khangman_static
    STATIC
    URI org.kde.khangman
    VERSION 1.0
)

ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX KHANGMAN VERSION_HEADER khangman_version.h)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDES}
)

target_sources(khangman_static PRIVATE
    khangman.cpp
    khangman.h
    khmtheme.cpp
    khmthemefactory.cpp
    khmthemefactory.h
    khmtheme.h
    langutils.cpp
    langutils.h
)
kconfig_add_kcfg_files(khangman_static prefs.kcfgc)

qt_target_qml_sources(khangman_static
    QML_FILES
        qml/GamePage.qml
        qml/LetterElement.qml
        qml/SelectionDialog.qml
        qml/Main.qml

        ## Settings
        qml/Settings/SettingsPage.qml
        qml/Settings/AboutKDEPage.qml
        qml/Settings/AboutPage.qml
)

qt_target_qml_sources(khangman_static
    PREFIX /
    RESOURCES
        # sounds
        qml/sounds/EW_Dialogue_Appear.ogg
        qml/sounds/EW_Dialogue_Appear.wav
        qml/sounds/chalk.ogg
        qml/sounds/chalk.wav
        qml/sounds/khangman-alphabet-button-press.wav
        qml/sounds/new_game.ogg
        qml/sounds/new_game.wav
        qml/sounds/splash.ogg
        qml/sounds/splash.wav
        qml/sounds/wrong.ogg
        qml/sounds/wrong.wav

        # gallows
        qml/gallows/gallows1.png
        qml/gallows/gallows10.png
        qml/gallows/gallows2.png
        qml/gallows/gallows3.png
        qml/gallows/gallows4.png
        qml/gallows/gallows5.png
        qml/gallows/gallows6.png
        qml/gallows/gallows7.png
        qml/gallows/gallows8.png
        qml/gallows/gallows9.png
)

target_link_libraries(khangman_static
PUBLIC
    Qt6::Core
    Qt6::Qml
    KF6::Crash
    KF6::I18n
    KF6::NewStuffCore
    KF6::ConfigCore
    KF6::WidgetsAddons
    LibKEduVocDocumentImport__KEduVocDocument
)

add_executable(khangman
    main.cpp
)

target_link_libraries(khangman PRIVATE khangman_static khangman_staticplugin)

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
    ${KDE_INSTALL_KNSRCDIR}
)

install(
    FILES org.kde.khangman.desktop
    DESTINATION ${KDE_INSTALL_APPDIR}
)

install(
    TARGETS khangman  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
)
