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
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2
import QtMultimedia 5.0
import QtQml 2.2

Item {

    id: gamePage

    property variant alphabet: khangman.alphabet
    property color currentWordLetterRectangleColor: Qt.rgba(0, 0, 0, 0)
    property int countDownTimerValue: khangman.resolveTime
    property int gallowsSeriesCounter: 0
    property bool initialized: false
    property alias isPlaying: secondTimer.running
    property string missedLetters: ""

    anchors.fill: parent

    function nextWord() {
        khangman.nextWord();

        countDownTimerValue = khangman.resolveTime;

        // Re enable all alphabet buttons
        for (var i = 0; i < alphabetLetterRepeater.count; ++i) {
            alphabetLetterRepeater.itemAt(i).enabled = true;
        }

        // Reset variables for the new word.
        gallowsSeriesCounter = 0;
        successImage.visible = false;
        missedLetters = "";

        hintLabel.visible = false;

        if (khangman.soundEnabled) {
            nextWordSoundEffect.play();
        }
    }

    function startTimer() {
        secondTimer.repeat = true;
        secondTimer.running = true;
        secondTimer.start();
    }

    MainSettingsDialog {
        id: mainSettingsDialog
        onOkClicked: {
            console.log("okCLicked() signal received")
            // close the settings dialog
            mainSettingsDialog.close()
            if (timerDisplay.visible) {
                // game is going on, so load a new word and start with the saved settings
                nextWord()
                startTimer()
            }
        }
        onCancelClicked: {
            console.log("cancelCLicked() signal received")
            // close the settings dialog
            mainSettingsDialog.close()
            if (timerDisplay.visible) {
                // game was in progress, so resume the timer countdown
                startTimer()
            }
        }
    }

    // Create a selection dialog with the vocabulary titles to choose from.
    MySelectionDialog {
        id: categorySelectionDialog;
        title: i18n("Choose the word category");
        model: khangman.categories
        selectedIndex: khangman.currentCategory

        onSelectedIndexChanged: {
            if (khangman.soundEnabled) {
                if (!initialized)
                    initialized = true;
                else
                    nextWordSoundEffect.play();
            }

            khangman.setCurrentCategory(selectedIndex);
            khangman.readFile();
            nextWord();
        }
    }

    // And another selection dialog with the languages to choose from.
    MySelectionDialog {
        id: languageSelectionDialog;
        title: i18n("Select a language");
        model: khangman.languages
        selectedIndex: khangman.currentLanguage
        onSelectedIndexChanged: {
            khangman.setCurrentLanguage(selectedIndex);
            khangman.readFile();
            nextWord();
        }
    }

    Timer {
        id: secondTimer;
        interval: 1000;
        repeat: true;
        running: false;
        triggeredOnStart: false;

        onTriggered: {
            if (khangman.resolveTime != 0 && --countDownTimerValue == 0) {
                stop();
                khangmanResultTimer.start();
                if (khangman.soundEnabled) {
                    wrongSoundEffect.play();
                }
            }
        }
    }

    Timer {
        id: khangmanResultTimer;
        interval: 1000;
        repeat: false;
        running: false;
        triggeredOnStart: false;

        onTriggered: {
            nextWord();
            startTimer();
        }
    }

    ToolBar {
        id: homePageTools
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        RowLayout {
            anchors.fill: parent

            ToolButton {
                id: settingsButton
                Layout.fillWidth: true
                iconSource: "settings_icon.png";
                tooltip: i18n("Settings.")

                onClicked: {
                    // if game is currently going on then pause it
                    if( gamePage.isPlaying ) {
                        secondTimer.repeat = false
                        secondTimer.running = false
                        hintLabel.visible = false
                        secondTimer.stop();
                    }
                    mainSettingsDialog.open()
                }
            }

            ToolButton {
                id: aboutKhangmanButton
                Layout.fillWidth: true
                iconSource: "dialog-information.png"
                tooltip: i18n("About KHangMan.")

                onClicked: {
                    khangman.showAboutKHangMan()
                }
            }

            ToolButton {
                id: aboutKDEButton
                Layout.fillWidth: true
                iconSource: "about-kde.png"
                tooltip: i18n("About KDE.")

                onClicked: {
                    khangman.showAboutKDE()
                }
            }

            ToolButton {
                id: showHandbookButton
                Layout.fillWidth: true
                iconSource: "handbook.png"
                tooltip: i18n("View the KHangMan Handbook.")

                onClicked: {
                    khangman.showHandbook()
                }
            }

            ToolButton {
                id: ghnsButton
                Layout.fillWidth: true
                iconSource: "get-hot-new-stuff.png"
                tooltip: i18n("Download new language files.")

                onClicked: {
                    khangman.slotDownloadNewStuff()
                }
            }
        }
    }

    // display the wrong guesses in a row
    Row {
        id: misses
        spacing: 5
        visible: isPlaying

        anchors {
            top: homePageTools.bottom
            horizontalCenter: parent.horizontalCenter
            topMargin: 5
        }

        Label {
            id: missesLabel
            text: "Misses- "
            font.pixelSize: 40
            font.bold: true
        }

        // display the missed alphabets stored in missedLetters variable
        Label {
            id: missedLetterText
            text: missedLetters
            font.pixelSize: 40
            font.bold: true
        }

        // display the remaining blanks
        Repeater {
            id: blank
            model: 10 - missedLetters.length
            Label {
                id: blankRepeater
                text: "_"
                font.pixelSize: 40
                font.bold: true
            }
        }
    }

    Image {
        id: successImage;
        source: "action-success.png";
        visible: false;

        anchors {
            horizontalCenter: parent.horizontalCenter;
            verticalCenter: parent.verticalCenter;
            verticalCenterOffset: -parent.height/4;
        }
    }

    // play/pause icon
    Image {
        id: playPauseButton
        source: gamePage.isPlaying ? "pause.png" : "play.png";
        visible: true

        anchors {
            right: parent.right
            bottom: timerDisplay.top
        }

        MouseArea {
            anchors.fill: playPauseButton
            onClicked: {
                if( gamePage.isPlaying ) { // game is currently going on, so pause it
                    secondTimer.repeat = false
                    secondTimer.running = false
                    hintLabel.visible = false
                    secondTimer.stop();
                } else {  // the game is paused or not yet started, so resume or start it
                    // if the game is not yet started, play nextWordSoundeffect
                    if (timerDisplay.visible == false) {
                        // denotes the game is not yet started, should return false if game is paused instead
                        if (khangman.soundEnabled) {
                            nextWordSoundEffect.play()
                        }
                    }

                    timerDisplay.visible = true
                    startTimer()
                }
            }
        }
    }

    Image {
        id: quitButton
        source: "quit.png"
        visible: true

        anchors {
            right: parent.right;
            top: homePageTools.bottom
        }

        MouseArea {
            anchors.fill: quitButton
            onClicked: {
                Qt.quit()
            }
        }
    }

    Image {
        id: gallowsSeriesImage;
        source: gallowsSeriesCounter == 0 ? "" : "gallows/gallows" + gallowsSeriesCounter + ".png"
        visible: (isPlaying && gallowsSeriesCounter > 0)

        anchors {
            horizontalCenter: parent.horizontalCenter;
            verticalCenter: parent.verticalCenter;
            verticalCenterOffset: -parent.height/4;
        }
    }

    Row {
        id: timerDisplay
        spacing: 5;
        visible: isPlaying

        anchors {
            right: parent.right;
            top: currentWordGrid.top
            topMargin: 5;
            rightMargin: 5;
        }

        LetterElement {
            letterText: Math.floor(countDownTimerValue / 60 / 10);
            visible: khangman.resolveTime == 0 ? false : true;
        }

        LetterElement {
            letterText: Math.floor(countDownTimerValue / 60 % 10);
            visible: khangman.resolveTime == 0 ? false : true;
        }

        LetterElement {
            letterText: ":";
            visible: khangman.resolveTime == 0 ? false : true;
        }

        LetterElement {
            letterText: Math.floor(countDownTimerValue % 60 / 10);
            visible: khangman.resolveTime == 0 ? false : true;
        }

        LetterElement {
            letterText: Math.floor(countDownTimerValue % 60 % 10);
            visible: khangman.resolveTime == 0 ? false : true;
        }
    }

    Grid {
        id: currentWordGrid;
        visible: gamePage.isPlaying
        anchors {
            centerIn: parent;
        }

        spacing: 5;
        columns: 13;
        Repeater {
            id: currentWordLetterRepeater;
            model: khangman.currentWord;
            LetterElement {
                id: currentWordLetterId;
                letterText: modelData;
            }
        }
    }

    Grid {
        id: alphabetGrid;
        visible: gamePage.isPlaying
        anchors {
            horizontalCenter: parent.horizontalCenter;
            bottom: mainPageTools.top;
            bottomMargin: 10;
        }

        spacing: gamePage.width/35;
        columns: 13;
        Repeater {
            id: alphabetLetterRepeater;
            model: alphabet;
            Button {
                id: alphabetLetterId;

                property string alphabetLetterIdLabel: modelData

                style: ButtonStyle {
                    id: alphabetLetterIdStyle
                    background: Rectangle {
                        id: alphabetLetterIdStyleRectangle
                        /*background: "image://theme/meegotouch-button-inverted-background";
                        fontFamily: "Arial";
                        fontPixelSize: 40;
                        fontCapitalization: Font.AllUppercase;
                        fontWeight: Font.Bold;
                        horizontalAlignment: Text.AlignHCenter;
                        textColor: "white";
                        pressedTextColor: "pink";
                        disabledTextColor: "gray";
                        checkedTextColor: "blue";
                        buttonWidth: 45;
                        buttonHeight: 60;*/
                        implicitWidth: gamePage.width / 22
                        implicitHeight: gamePage.width / 22
                        color: alphabetLetterId.enabled ? "black" : "grey"
                        radius: 8
                    }
                    label: Text {
                        id: buttonLabel
                        anchors.centerIn: parent
                        text: alphabetLetterId.alphabetLetterIdLabel
                        font.family : "Arial"
                        font.pixelSize: gamePage.width / 40
                        font.capitalization : Font.AllUppercase
                        font.weight : Font.Bold
                        horizontalAlignment : Text.AlignHCenter
                        verticalAlignment : Text.AlignVCenter
                        color: "white"
                    }
                }

                onClicked: {
                    if (khangman.soundEnabled) {
                        khangmanAlphabetButtonPressSoundEffect.play();
                    }

                    if (khangman.containsChar(alphabetLetterId.alphabetLetterIdLabel)) {
                        khangman.replaceLetters(alphabetLetterId.alphabetLetterIdLabel);
                        enabled = false;

                        if (khangman.isResolved()) {
                            successImage.visible = true;
                            khangmanResultTimer.start();

                            if (khangman.soundEnabled) {
                                ewDialogAppearSoundEffect.play();
                            }
                        }
                    } else {
                        enabled = false;
                        if (gallowsSeriesCounter++ == 9) {
                            if (khangman.soundEnabled) {
                                wrongSoundEffect.play();
                            }

                            khangmanResultTimer.start();
                        }

                        missedLetters += alphabetLetterId.alphabetLetterIdLabel
                    }
                }
            }
        }
    }

    Label {
        id: hintLabel
        text: khangman.currentHint
        font.family: "serif-sans"
        color: "green"
        font.italic: true
        font.pixelSize: gamePage.width / 60
        anchors.top: currentWordGrid.bottom
        anchors.bottom: alphabetGrid.top
        anchors.horizontalCenter: parent.horizontalCenter
        visible: false
    }

    ToolBar {
        id: mainPageTools
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        visible: isPlaying

        RowLayout {
            anchors.fill: parent

            ToolButton {
                id: helpHintButton
                iconSource: "help-hint.png"
                tooltip: i18n("Display the hint.")
                enabled: hintLabel.text != ""

                onClicked: {
                    // make the button toggle between display and hide the hint
                    hintLabel.visible = hintLabel.visible ? false : true
                    //console.log("hintLabel.font.family = " + hintLabel.font.family)
                }
            }

            ToolButton {
                id: categorySelectionButton
                Layout.fillWidth: true
                text: categorySelectionDialog.model[categorySelectionDialog.selectedIndex];
                tooltip: i18n("Change the category.")

                onClicked: {
                    categorySelectionDialog.open();
                }
            }

            ToolButton {
                id: languageSelectionButton
                Layout.fillWidth: true
                text: languageSelectionDialog.model[languageSelectionDialog.selectedIndex]
                tooltip: i18n("Change the language.")

                onClicked: {
                    languageSelectionDialog.open()
                }
            }

            ToolButton {
                id: nextWordButton
                Layout.fillWidth: true
                iconSource: "go-next.png";
                tooltip: i18n("Load the next word and start a new game.")

                onClicked: {
                    if (khangman.soundEnabled) {
                        //console.log("kahngman.sound = true")
                        //console.log("checking sound effect loaded" + nextWordSoundeffect.isLoaded());
                        nextWordSoundEffect.play();
                    } else {
                        //console.log("khangman.soundEnabled = false")
                    }

                    nextWord();

                    secondTimer.repeat = true;
                    secondTimer.restart();
                }
            }
        }
    }
}
