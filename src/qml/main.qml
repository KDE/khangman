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

import QtQuick 2.3
//import com.nokia.meego 1.0
//import QtMultimediaKit 1.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

StackView {
    id: rootWindow;

    // MainPage is what we see when the app starts, it shows up
    // the available games on the mobile handset
    initialItem: mainPage;

    Image {
        id: backgroundImage
        smooth: true
        fillMode: Image.TileVertically
        anchors.fill: parent
        source: "khangman-background-landscape.png"
    }

    MainPage {
        id: mainPage
    }

    GamePage {
        id: gamePage;
        visible: false
    }

    MainSettingsPage {
        id: mainSettingsPage;
        visible: false
    }

    // These tools are shared by most sub-pages by assigning the
    // id to a page's tools property

    ToolBar {
        id: commonTools;
        visible: (rootWindow.currentItem != mainPage)
        RowLayout {
            ToolButton {
                iconSource: "go-back.png";
                onClicked: {
                    if (rootWindow.currentPage == mainSettingsPage) {
                        khangmanEngineHelper.saveSettings();
                    }

                    rootWindow.pop();
                }
                tooltip: i18n("Go back to Main Page.")
            }
        }
    }

    Component.onCompleted: {
        // Use the dark theme.
        //theme.inverted = true;
    }

    /*SoundEffect {
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
    }*/
}
