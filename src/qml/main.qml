/***********************************************************************************
 * This file is part of the KHangMan project                                       *
 * Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>                                  *
 * Copyright (C) 2014 Rahul Chowdhury <rahul.chowdhury@kdemail.net>                *
 *                                                                                 *
 * This library is free software; you can redistribute it and/or                   *
 * modify it under the terms of the GNU Lesser General Public                      *
 * License as published by the Free Software Foundation; either                    *
 * version 2.1 of the License, or (at your option) any later version.              *
 *                                                                                 *
 * This library is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                  *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU               *
 * Lesser General Public License for more details.                                 *
 *                                                                                 *
 * You should have received a copy of the GNU Lesser General Public                *
 * License along with this library; if not, write to the Free Software             *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA  *
 ***********************************************************************************/

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtMultimedia 5.0

Rectangle {
    id: rootWindow;

    Image {
        id: backgroundImage
        smooth: true
        fillMode: Image.TileVertically
        anchors.fill: parent
        source: "khangman-background-landscape.png"
    }

    GamePage {
        id: gamePage
    }

    MainSettingsDialog {
        id: mainSettingsDialog
        visible: false
    }

    Component.onCompleted: {
        // play this audio file during startup
        if (khangman.soundEnabled) {
            new_gameSoundEffect.play()
        }
    }

    SoundEffect {
        id: ewDialogAppearSoundEffect;
        source: "sounds/EW_Dialogue_Appear.wav";
    }

    SoundEffect {
        id: nextWordSoundEffect;
        source: "sounds/chalk.wav";
    }

    SoundEffect {
        id: wrongSoundEffect;
        source: "sounds/wrong.wav";
    }

    SoundEffect {
        id: new_gameSoundEffect;
        source: "sounds/new_game.wav";
    }

    SoundEffect {
        id: khangmanAlphabetButtonPressSoundEffect;
        source: "sounds/khangman-alphabet-button-press.wav";
    }
}
