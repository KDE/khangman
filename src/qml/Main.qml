// SPDX-FileCopyrightText: 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-FileCopyrightText: 2014 Rahul Chowdhury <rahul.chowdhury@kdemail.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick 2.3
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.khangman
import QtMultimedia

Kirigami.ApplicationWindow {
    id: rootWindow;

    pageStack.initialPage: GamePage {
        id: gamePage
    }

    Component.onCompleted: {
        // play this audio file during startup
        if (KHangMan.soundEnabled) {
            new_gameSoundEffect.play()
        }
    }

    SoundEffect {
        id: ewDialogAppearSoundEffect;
        source: "qrc:/qml/sounds/EW_Dialogue_Appear.wav";
    }

    SoundEffect {
        id: nextWordSoundEffect;
        source: "qrc:/qml/sounds/chalk.wav";
    }

    SoundEffect {
        id: wrongSoundEffect;
        source: "qrc:/qml/sounds/wrong.wav";
    }

    SoundEffect {
        id: new_gameSoundEffect;
        source: "qrc:/qml/sounds/new_game.wav";
    }

    SoundEffect {
        id: khangmanAlphabetButtonPressSoundEffect;
        source: "qrc:/qml/sounds/khangman-alphabet-button-press.wav";
    }

    Connections {
        target: KHangMan

        function onErrorOccured(errorMessage: string): void {
            rootWindow.showPassiveNotification(i18n("Error: %1", errorMessage));
        }
    }
}
