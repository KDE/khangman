/******************************************************************************
 * This file is part of the KHangMan project
 * Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

import QtQuick 1.1
import com.nokia.meego 1.0
import QtMultimediaKit 1.1

PageStackWindow {
    id: rootWindow;

    // MainPage is what we see when the app starts, it shows up
    // the available games on the mobile handset
    initialPage: mainPage;

    MainPage {
        id: mainPage;
    }

    GamePage {
        id: gamePage;
    }

    MainSettingsPage {
        id: mainSettingsPage;
    }

    // These tools are shared by most sub-pages by assigning the
    // id to a page's tools property
    ToolBarLayout {
        id: commonTools;
        visible: false;
        ToolIcon {
            iconId: "toolbar-back";
            onClicked: {
                if (pageStack.currentPage == mainSettingsPage) {
                    khangmanEngineHelper.saveSettings();
                }

                pageStack.pop();
            }
        }
    }

    Component.onCompleted: {
        // Use the dark theme.
        theme.inverted = true;
    }

    platformStyle: PageStackWindowStyle {
        // Note: It is needed for being backward compatible with PR1.0 where
        // "foobar.png" does not work as expected. It was fixed in later
        // versions though, but the Nokia Ovi Store requires backward
        // compatibility with PR1.0
        background: "qrc:/khangman-background-landscape.png";
        landscapeBackground: "qrc:/khangman-background-landscape.png";
        portraitBackground: "qrc:/khangman-background-portrait.png";
        backgroundFillMode: Image.Tile;
    }

    SoundEffect {
        id: ewDialogAppearSoundEffect;
        source: "EW_Dialogue_Appear.wav";
    }

    SoundEffect {
        id: nextWordSoundEffect;
        source: "chalk.wav";
    }

    SoundEffect {
        id: wrongSoundEffect;
        source: "wrong.wav";
    }

    SoundEffect {
        id: splashSoundEffect;
        source: "splash.wav";
    }

    SoundEffect {
        id: khangmanAlphabetButtonPressSoundEffect;
        source: "khangman-alphabet-button-press.wav";
    }
}
