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
//import QtMultimediaKit 1.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2
import QtMultimedia 5.0
import QtQml 2.2

//import com.nokia.meego 1.0
//import com.nokia.extras 1.0

Item {

    id: gamePage

    //property variant currentWord: khangman.currentWordLetters();
    property variant alphabet: khangman.alphabet();
    property color currentWordLetterRectangleColor: Qt.rgba(0, 0, 0, 0);
    property int countDownTimerValue: khangman.resolveTime;
    property int gallowsSeriesCounter: 0;
    property bool initialized: false;
    property alias isPlaying: secondTimer.running

    /*onStatusChanged: {
        if (status == PageStatus.Active) {
            secondTimer.repeat = true;
            secondTimer.restart();
        }
    }*/

    Connections {
        //target: platformWindow;

        /*onActiveChanged: {
            if (platformWindow.active && status == PageStatus.Active) {
                secondTimer.repeat = true;
                secondTimer.restart();
            } else {
                khangmanHintInfoBanner.hide();

                secondTimer.repeat = false;
                secondTimer.stop();
            }
        }*/
    }

    //state: (screen.currentOrientation == Screen.Portrait || screen.currentOrientation == Screen.PortraitInverted) ? "portrait" : "landscape"

    states: [
        State {
            name: "landscape"
            PropertyChanges { target: alphabetGrid; columns: 13; rows: 2 }
            PropertyChanges { target: currentWordGrid; columns: 13; }
        },

        State {
            name: "portrait"
            PropertyChanges { target: alphabetGrid; columns: 9; rows: 3 }
            PropertyChanges { target: currentWordGrid; columns: 9; }
        }
    ]

    Component.onCompleted: {
        categorySelectionDialog.selectedIndex = khangman.currentLevel();
    }

    function pushPage(file) {
        var component = Qt.createComponent(file)
        if (component.status == Component.Ready)
            rootWindow.push(component);
        else
            console.log(i18n("Error loading component:", component.errorString()));
    }

    function nextWord() {
        //khangmanHintInfoBanner.hide();
        khangman.nextWord();

        //currentWord = khangman.currentWordLetters();
        countDownTimerValue = khangman.resolveTime;

        for (var i = 0; i < alphabetLetterRepeater.count; ++i) {
            alphabetLetterRepeater.itemAt(i).enabled = true;
        }

        gallowsSeriesCounter = 0;
        gallowsSeriesImage.visible = false;
        successImage.visible = false;

        hintLabel.visible = false

        if (rootWindow.currentItem == gamePage) {
            //console.log("nextWordSoundEffect.status = " + nextWordSoundEffect.status)
            //console.log("checking sound effect loaded " + nextWordSoundEffect.isLoaded());
            if (khangman.sound) {
                nextWordSoundEffect.play()
                //console.log("nextWordSoundEffect.play()")
            }
            else {
                //console.log("khangman.sound = false in nextWord()")
            }
        }
    }

    function startTimer() {
        secondTimer.repeat = true;
        secondTimer.running = true;
        secondTimer.start();
    }

    // Create an info banner with icon
    /*InfoBanner {
        id: khangmanHintInfoBanner;
        text: i18n("No hint available");
        iconSource: "dialog-information.png";

        topMargin: 5;
    }*/

    // Create a selection dialog with the vocabulary titles to choose from.
    MySelectionDialog {
        id: categorySelectionDialog;
        title: i18n("Choose the word category");
        model: khangman.categoryList();

        onSelectedIndexChanged: {

            if (khangman.sound) {
                //console.log("khangman.sound = " + khangman.sound)
                initialized == true ? nextWordSoundEffect.play() : initialized = true;
            } else {
                //console.log("khangman.sound = false")
            }

            khangman.selectCurrentLevel(selectedIndex);
            khangman.selectLevelFile(selectedIndex);
            khangman.saveSettings();

            khangman.readFile();
            nextWord();
        }
    }

    // These tools are available for the main page by assigning the
    // id to the main page's tools property

    //tools: mainPageTools;

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
                if (khangman.sound) {
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

            /*secondTimer.repeat = true;
            secondTimer.start();*/

            startTimer();
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
        source: gamePage.isPlaying ? "pause.png" : "play.png"
        visible: true

        anchors {
            right: parent.right;
            bottom: timerDisplay.top
        }

        MouseArea {
            anchors.fill: playPauseButton
            onClicked: {
                //rootWindow.push(gamePage)
                if( gamePage.isPlaying ) { // game is currently going on, so pause it
                    //console.log("isPlaying = " + gamePage.isPlaying)
                    secondTimer.repeat = false
                    secondTimer.running = false
                    mainPageTools.visible = false
                    hintLabel.visible = false
                    secondTimer.stop();
                } else {  // the game is paused or not yet started, so resume or start it 
                    //console.log("isPlaying = " + gamePage.isPlaying)

                    // if the game is not yet started, play nextWordSoundeffect
                    if (timerDisplay.visible == false) {
                        // denotes the game is not yet started, should return false if game is paused instead
                        if (khangman.sound) {
                            nextWordSoundEffect.play()
                            //console.log("nextWordSoundEffect.play()")
                        }
                    }

                    timerDisplay.visible = true
                    mainPageTools.visible = true
                    startTimer()
                    //gamePage.isPlaying = secondTimer.running
                }
            }
        }
    }

    Image {
        id: gallowsSeriesImage;
        source: gallowsSeriesCounter == 0 ? "" : "gallows/gallows" + gallowsSeriesCounter + ".png";
        visible: false;

        anchors {
            horizontalCenter: parent.horizontalCenter;
            verticalCenter: parent.verticalCenter;
            verticalCenterOffset: -parent.height/4;
        }
    }

    Row {
        id: timerDisplay
        spacing: 5;
        visible: false

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
            //top: currentWordGrid.bottom
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
                //text: modelData;

                property string alphabetLetterIdLabel: modelData

                style: ButtonStyle {
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
                    if (khangman.sound) {
                        khangmanAlphabetButtonPressSoundEffect.play();
                    }

                    if (khangman.containsChar(alphabetLetterId.alphabetLetterIdLabel)) {
                        khangman.replaceLetters(alphabetLetterId.alphabetLetterIdLabel);
                        //currentWord = khangman.currentWordLetters();
                        enabled = false;

                        if (khangman.isResolved()) {
                            gallowsSeriesImage.visible = false;
                            successImage.visible = true;
                            khangmanResultTimer.start();
                            //khangmanHintInfoBanner.hide();

                            if (khangman.sound) {
                                ewDialogAppearSoundEffect.play();
                            }
                        }
                    } else {
                        enabled = false;

                        if (gallowsSeriesCounter++ == 0) {
                            gallowsSeriesImage.visible = true;
                        }

                        if (gallowsSeriesCounter == 10) {
                            if (khangman.sound) {
                                wrongSoundEffect.play();
                            }

                            khangmanResultTimer.start();
                        }
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
        //font.italic: true
        anchors.top: currentWordGrid.bottom
        anchors.bottom: alphabetGrid.top
        anchors.horizontalCenter: parent.horizontalCenter
        visible: false
    }

    ToolBar {
        id: mainPageTools;
        //anchors.top: alphabetGrid.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        visible: false

        RowLayout {
            anchors.fill: parent

            ToolButton {
                iconSource: "help-hint.png";
                enabled: hintLabel.text != ""

                onClicked: {
                    // make the button toggle between display and hide the hint
                    hintLabel.visible = hintLabel.visible ? false : true
                    //console.log("hintLabel.font.family = " + hintLabel.font.family)
                }
            }

            ToolButton {
                text: categorySelectionDialog.model[categorySelectionDialog.selectedIndex];

                anchors {
                    centerIn: parent;
                }

                onClicked: {
                    categorySelectionDialog.open();
                }
            }

            /*ToolButton {
                iconSource: "timer-pause.png";

                anchors {
                    horizontalCenter: parent.horizontalCenter;
                    horizontalCenterOffset: parent.width/4;
                }

                onClicked: {
                    //khangmanHintInfoBanner.hide();

                    rootWindow.pop();

                    secondTimer.repeat = false;
                    secondTimer.stop();
                }
            }*/

            ToolButton {
                iconSource: "go-next.png";

                onClicked: {
                    if (khangman.sound) {
                        //console.log("kahngman.sound = true")
                        //console.log("checking sound effect loaded" + nextWordSoundeffect.isLoaded());
                        nextWordSoundEffect.play();
                    } else {
                        //console.log("khangman.sound = false")
                    }

                    nextWord();
                    /*secondTimer.repeat = true;
                    secondTimer.restart();*/

                    secondTimer.repeat = true;
                    secondTimer.restart();
                }
            }
        }
    }
}
