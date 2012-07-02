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
import QtMultimediaKit 1.1

import com.nokia.meego 1.0
import com.nokia.extras 1.0

Page {

    property variant currentWord: khangmanEngineHelper.currentWordLetters();
    property variant alphabet: khangmanEngineHelper.alphabet();
    property color currentWordLetterRectangleColor: Qt.rgba(0, 0, 0, 0);
    property int countDownTimerValue: khangmanEngineHelper.resolveTime;
    property int gallowsSeriesCounter: 0;
    property bool initialized: false;

    onStatusChanged: {
        if (status == PageStatus.Active) {
            secondTimer.repeat = true;
            secondTimer.restart();
        }
    }

    Connections {
        target: platformWindow;

        onActiveChanged: {
            if (platformWindow.active && status == PageStatus.Active) {
                secondTimer.repeat = true;
                secondTimer.restart();
            } else {
                khangmanHintInfoBanner.hide();

                secondTimer.repeat = false;
                secondTimer.stop();
            }
        }
    }

    state: (screen.currentOrientation == Screen.Portrait || screen.currentOrientation == Screen.PortraitInverted) ? "portrait" : "landscape"

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
        categorySelectionDialog.selectedIndex = khangmanEngine.currentLevel();
    }

    function pushPage(file) {
        var component = Qt.createComponent(file)
        if (component.status == Component.Ready)
            pageStack.push(component);
        else
            console.log(i18n("Error loading component:", component.errorString()));
    }

    function nextWord() {
        khangmanHintInfoBanner.hide();
        khangmanEngine.nextWord();
        currentWord = khangmanEngineHelper.currentWordLetters();
        countDownTimerValue = khangmanEngineHelper.resolveTime;

        for (var i = 0; i < alphabetLetterRepeater.count; ++i) {
            alphabetLetterRepeater.itemAt(i).enabled = true;
        }

        gallowsSeriesCounter = 0;
        gallowsSeriesImage.visible = false;
        successImage.visible = false;
    }

    // Create an info banner with icon
    InfoBanner {
        id: khangmanHintInfoBanner;
        text: i18n("There is no hint available for this word");
        iconSource: "dialog-information.png";

        topMargin: 5;
    }

    // Create a selection dialog with the vocabulary titles to choose from.
    MySelectionDialog {
        id: categorySelectionDialog;
        titleText: i18n("Choose the word category");

        model: khangmanEngine.categoryList();

        onSelectedIndexChanged: {

            if (khangmanEngineHelper.sound) {
                initialized == true ? nextWordSoundEffect.play() : initialized = true;
            }

            khangmanEngine.selectCurrentLevel(selectedIndex);
            khangmanEngine.selectLevelFile(selectedIndex);
            khangmanEngineHelper.saveSettings();

            khangmanEngine.readFile();
            nextWord();
        }
    }

    // These tools are available for the main page by assigning the
    // id to the main page's tools property
    ToolBarLayout {
        id: mainPageTools;
        visible: false;

        ToolIcon {
            iconSource: "help-hint.png";

            onClicked: {
                khangmanHintInfoBanner.text = khangmanEngine.hint();
                khangmanHintInfoBanner.timerShowTime = khangmanEngineHelper.hintHideTime * 1000;

                // Display the info banner
                khangmanHintInfoBanner.show();
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

        ToolIcon {
            iconSource: "timer-pause.png";

            anchors {
                horizontalCenter: parent.horizontalCenter;
                horizontalCenterOffset: parent.width/4;
            }

            onClicked: {
                khangmanHintInfoBanner.hide();

                pageStack.pop();

                secondTimer.repeat = false;
                secondTimer.stop();

            }
        }

        ToolIcon {
            iconSource: "go-next.png";

            onClicked: {
                if (khangmanEngineHelper.sound) {
                    nextWordSoundEffect.play();
                }

                nextWord();
                secondTimer.repeat = true;
                secondTimer.restart();
            }
        }
    }

    tools: mainPageTools;

    Timer {
        id: secondTimer;
        interval: 1000;
        repeat: true;
        running: false;
        triggeredOnStart: false;

        onTriggered: {
             if (khangmanEngineHelper.resolveTime != 0 && --countDownTimerValue == 0) {
                 stop();
                 khangmanResultTimer.start();
                 if (khangmanEngineHelper.sound) {
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

            secondTimer.repeat = true;
            secondTimer.start();
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

    Image {
        id: gallowsSeriesImage;
        source: gallowsSeriesCounter == 0 ? "" : "gallows" + gallowsSeriesCounter + ".png";
        visible: false;

        anchors {
            horizontalCenter: parent.horizontalCenter;
            verticalCenter: parent.verticalCenter;
            verticalCenterOffset: -parent.height/4;
        }
    }

    Row {
        spacing: 5;

        anchors {
            right: parent.right;
            top: parent.top;
            topMargin: 5;
            rightMargin: 5;
        }

        LetterElement {
            letterText: Math.floor(countDownTimerValue / 60 / 10);
            visible: khangmanEngineHelper.resolveTime == 0 ? false : true;
        }

        LetterElement {
            letterText: Math.floor(countDownTimerValue / 60 % 10);
            visible: khangmanEngineHelper.resolveTime == 0 ? false : true;
        }

        LetterElement {
            letterText: ":";
            visible: khangmanEngineHelper.resolveTime == 0 ? false : true;
        }

        LetterElement {
            letterText: Math.floor(countDownTimerValue % 60 / 10);
            visible: khangmanEngineHelper.resolveTime == 0 ? false : true;
        }

        LetterElement {
            letterText: Math.floor(countDownTimerValue % 60 % 10);
            visible: khangmanEngineHelper.resolveTime == 0 ? false : true;
        }
    }

    Grid {
        id: currentWordGrid;
        anchors {
            centerIn: parent;
        }

        spacing: 5;
        Repeater {
            id: currentWordLetterRepeater;
            model: currentWord;
            LetterElement {
                id: currentWordLetterId;
                letterText: modelData;
            }
        }
    }

    Grid {
        id: alphabetGrid;
        anchors {
            horizontalCenter: parent.horizontalCenter;
            bottom: parent.bottom;
            bottomMargin: 10;
        }

        spacing: 5;
        Repeater {
            id: alphabetLetterRepeater;
            model: alphabet;
            Button {
                id: alphabetLetterId;
                text: modelData;

                platformStyle: ButtonStyle {
                    background: "image://theme/meegotouch-button-inverted-background";
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
                    buttonHeight: 60;
                }

                onClicked: {
                    if (khangmanEngineHelper.sound) {
                        khangmanAlphabetButtonPressSoundEffect.play();
                    }

                    if (khangmanEngine.containsChar(text)) {
                        khangmanEngine.replaceLetters(text);
                        currentWord = khangmanEngineHelper.currentWordLetters();
                        enabled = false;

                        if (khangmanEngine.isResolved()) {
                            gallowsSeriesImage.visible = false;
                            successImage.visible = true;
                            khangmanResultTimer.start();
                            khangmanHintInfoBanner.hide();

                            if (khangmanEngineHelper.sound) {
                                ewDialogAppearSoundEffect.play();
                            }
                        }
                    } else {
                        enabled = false;

                        if (gallowsSeriesCounter++ == 0) {
                            gallowsSeriesImage.visible = true;
                        }

                        if (gallowsSeriesCounter == 10) {
                            if (khangmanEngineHelper.sound) {
                                wrongSoundEffect.play();
                            }

                            khangmanResultTimer.start();
                        }
                    }
                }
            }
        }
    }
}
