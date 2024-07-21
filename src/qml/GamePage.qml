// SPDX-FileCopyrightText: 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-FileCopyrightText: 2014 Rahul Chowdhury <rahul.chowdhury@kdemail.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtMultimedia
import QtQml

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.newstuff as NewStuff
import org.kde.khangman

Kirigami.Page {
    id: gamePage

    focus: true

    property variant alphabet: KHangMan.alphabet
    property color currentWordLetterRectangleColor: Qt.rgba(0, 0, 0, 0)
    property int countDownTimerValue: KHangMan.resolveTime
    property int gallowsSeriesCounter: 0
    property bool initialized: false
    property alias isPlaying: secondTimer.running
    property string missedLetters: ""

    background: Image {
        id: backgroundImage
        smooth: true
        anchors.fill: parent
        source: KHangMan.backgroundUrl
    }

    function nextWord(): void {
        KHangMan.nextWord();

        countDownTimerValue = KHangMan.resolveTime;

        // Re enable all alphabet buttons
        for (var i = 0; i < alphabetLetterRepeater.count; ++i) {
            alphabetLetterRepeater.itemAt(i).enabled = true;
            alphabetLetterRepeater.itemAt(i).buttonColor = "black";
        }

        // Reset variables for the new word.
        gallowsSeriesCounter = 0;
        successImage.visible = false;
        missedLetters = "";

        hintLabel.visible = false;

        if (KHangMan.soundEnabled) {
            nextWordSoundEffect.play();
        }
    }

    function startTimer(): void {
        secondTimer.repeat = true;
        secondTimer.running = true;
        secondTimer.start();
    }

    function disableLetterButton(letter: string): void {
        for (var i = 0; i < alphabetLetterRepeater.count; ++i) {
            if (alphabetLetterRepeater.itemAt(i).upperCase === letter) {
                alphabetLetterRepeater.itemAt(i).enabled = false;
                break;
            }
        }
    }

    function guessLetter(letter: string): void {
        letter = letter.toUpperCase()
        if (KHangMan.soundEnabled) {
            khangmanAlphabetButtonPressSoundEffect.play();
        }

        disableLetterButton(letter);
        changeButtonColor(letter);
        if (KHangMan.containsChar(letter)) {
            KHangMan.replaceLetters(letter);

            if (KHangMan.isResolved()) {
                // the current puzzle is solved
                KHangMan.winCount++;
                successImage.visible = true;
                khangmanResultTimer.start();

                if (KHangMan.soundEnabled) {
                    ewDialogAppearSoundEffect.play();
                }
            }
        } else {
            // Only add to missedLetters if it's not already there
            if (missedLetters.indexOf(letter) == -1) {
                if (gallowsSeriesCounter++ == 9) {
                    // wrong solution given for current puzzle
                    KHangMan.lossCount++;
                    if (KHangMan.soundEnabled) {
                        wrongSoundEffect.play();
                    }

                    khangmanResultTimer.start();
                }

                missedLetters += letter
            }
        }
    }

    function changeButtonColor(letter: string): void {
        for (var i = 0; i < alphabetLetterRepeater.count; ++i) {
            if (alphabetLetterRepeater.itemAt(i).upperCase === letter) {
                alphabetLetterRepeater.itemAt(i).buttonColor = KHangMan.containsChar(letter) ? "green" : "red";
            }
        }
    }

    SelectionDialog {
        id: categorySelectionDialog;

        title: i18n("Choose the word category");
        model: KHangMan.categories
        Component.onCompleted: {
            currentIndex = KHangMan.currentCategory;
        }

        onCurrentIndexChanged: {
            if (KHangMan.soundEnabled) {
                if (!initialized) {
                    initialized = true;
                } else {
                    nextWordSoundEffect.play();
                }
            }

            KHangMan.setCurrentCategory(currentIndex);
            KHangMan.readFile();
            nextWord();
        }

        delegate: Delegates.RoundedItemDelegate {
            required property int index
            required property string modelData

            text: modelData
            onClicked: {
                categorySelectionDialog.currentIndex = index
                categorySelectionDialog.close();
            }
        }
    }

    SelectionDialog {
        id: languageSelectionDialog;

        title: i18n("Select a language");
        model: KHangMan.languages
        Component.onCompleted: currentIndex = KHangMan.currentLanguage

        delegate: Delegates.RoundedItemDelegate {
            required property int index
            required property string modelData

            text: modelData

            onClicked: {
                languageSelectionDialog.currentIndex = index;

                KHangMan.setCurrentLanguage(index);
                KHangMan.readFile();
                nextWord();
                languageSelectionDialog.close();
            }
        }
    }

    SelectionDialog {
        id: themeSelectionDialog;
        title: i18n("Select a theme");
        model: KHangMan.themes
        Component.onCompleted: currentIndex = KHangMan.currentTheme

        delegate: Delegates.RoundedItemDelegate {
            required property int index
            required property string modelData

            text: modelData

            onClicked: {
                themeSelectionDialog.currentIndex = index;

                KHangMan.setCurrentTheme(index);
                themeSelectionDialog.close();
            }
        }
    }

    Timer {
        id: secondTimer;
        interval: 1000;
        repeat: true;
        running: false;
        triggeredOnStart: false;

        onTriggered: {
            if (KHangMan.resolveTime !== 0 && --countDownTimerValue == 0) {
                stop();
                khangmanResultTimer.start();
                if (KHangMan.soundEnabled) {
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

    actions: [
        Kirigami.Action {
            id: playPauseButton
            icon.name: gamePage.isPlaying ? "media-playback-pause" : "media-playback-start"

            text: gamePage.isPlaying ? i18n("Pause") : i18n("Play")

            onTriggered: {
                if (gamePage.isPlaying ) { // game is currently going on, so pause it
                    secondTimer.repeat = false
                    secondTimer.running = false
                    hintLabel.visible = false
                    secondTimer.stop();
                } else {  // the game is paused or not yet started, so resume or start it
                    // if the game is not yet started, play nextWordSoundeffect
                    // denotes the game is not yet started, should return false if game is paused instead
                    if (KHangMan.soundEnabled) {
                        nextWordSoundEffect.play()
                    }
                    startTimer()
                }
            }
        },

        Kirigami.Action {
            id: themeSelectionButton
            text: themeSelectionDialog.model[themeSelectionDialog.currentIndex]

            onTriggered: {
                themeSelectionDialog.open()
            }
        },

        Kirigami.Action {
            id: settingsButton
            icon.name: "settings-configure-symbolic"
            text: i18nc("@action:button", "Configure")

            property bool wasPlaying: false

            onTriggered: {
                // if game is currently going on then pause it
                settingsButton.wasPlaying = isPlaying
                if( gamePage.isPlaying ) {
                    secondTimer.repeat = false
                    secondTimer.running = false
                    hintLabel.visible = false
                    secondTimer.stop();
                }

                const item = applicationWindow().pageStack.pushDialogLayer(Qt.createComponent("org.kde.khangman", "SettingsPage"), {}, {title: i18n("Configure"), width: Kirigami.Units.gridUnit * 24});

                item.okClicked.connect(() => {
                    // close the settings dialog
                    if (wasPlaying) {
                        // game is going on, so load a new word and start with the saved settings
                        nextWord()
                        startTimer()
                    }
                });

                item.cancelClicked.connect(() => {
                    if (wasPlaying) {
                        // game was in progress, so resume the timer countdown
                        startTimer()
                    }
                });
            }
        },

        NewStuff.Action {
            id: ghnsButton

            configFile: "khangman.knsrc"
            icon.name: "get-hot-new-stuff"
            text: i18n("Get new language files")
            visible: NewStuff.Settings.allowedByKiosk

            onEntryEvent: (entry, event) => {
                if (event === NewStuff.Entry.StatusChangedEvent) {
                    KHangMan.slotDownloadNewStuff(entry)
                }
            }
        },

        Kirigami.Action {
            id: showHandbookButton
            icon.name: "help-browser"
            text: i18n("View the KHangMan Handbook")
            displayHint: Kirigami.DisplayHint.AlwaysHide

            onTriggered: {
                KHangMan.showHandbook()
            }
        },

        Kirigami.Action {
            id: aboutKhangmanButton
            icon.name: "khangman"
            text: i18n("About KHangMan")
            displayHint: Kirigami.DisplayHint.AlwaysHide

            onTriggered: {
                applicationWindow().pageStack.pushDialogLayer("qrc:/qt/qml/org/kde/khangman/qml/Settings/AboutPage.qml", {} , {
                    width: Kirigami.Units.gridUnit * 24,
                    title: i18n("About KHangMan")
                })
            }
        },

        Kirigami.Action {
            id: aboutKDEButton
            text: i18n("About KDE")
            icon.name: "help-about-symbolic"
            displayHint: Kirigami.DisplayHint.AlwaysHide

            onTriggered: {
                applicationWindow().pageStack.pushDialogLayer("qrc:/qt/qml/org/kde/khangman/qml/Settings/AboutKDEPage.qml", {}, {
                    width: Kirigami.Units.gridUnit * 24,
                    title: i18n("About KDE")
                })
            }
        }
    ]

    // display the remaining number of wrong guesses
    Row {
        id: misses
        spacing: 5
        visible: isPlaying

        anchors {
            top: parent.top
            topMargin: 5
            right: parent.right
            rightMargin: 5
        }

        Label {
            id: missesLabel
            text: i18n("Remaining guesses: ")
            font.pixelSize: 40
            font.bold: true
            color: KHangMan.letterColor
        }

        Text {
            id: remainingGuessesCount
            text: i18n(10 - gallowsSeriesCounter)
            color: gallowsSeriesCounter >= 7 ? "red" : "black"
            font.pixelSize: 40
            font.bold: true
        }
    }

    Label {
        id: scoreLabel
        visible: isPlaying

        anchors {
            top: misses.top
            left: parent.left
            leftMargin: 5
        }

        text: i18n("Score: ")
        font.pixelSize: 40
        font.bold: true
        color: KHangMan.letterColor
    }

    Label {
        id: netScoreLabel
        visible: isPlaying

        anchors {
            left: scoreLabel.right
            top: scoreLabel.top
        }

        text: KHangMan.netScore
        color: KHangMan.netScore < 0 ? "red" : "black"
        font.pixelSize: 40
        font.bold: true
    }

    Row {
        id: winCountRow
        spacing: 15
        visible: isPlaying

        anchors {
            top: scoreLabel.bottom
            left: parent.left
            leftMargin: 5
        }

        Label {
            id: winLabel
            text: i18n("Wins: ")
            font.pixelSize: 40
            font.bold: true
            color: KHangMan.letterColor
        }

        Label {
            id: winCountLabel
            text: KHangMan.winCount
            font.pixelSize: 40
            font.bold: true
            color: KHangMan.letterColor
        }
    }

    Row {
        id: lossCountRow
        spacing: 15
        visible: isPlaying

        anchors {
            top: winCountRow.bottom
            left: parent.left
            leftMargin: 5
        }

        Label {
            id: lossLabel
            text: i18n("Losses: ")
            font.pixelSize: 40
            font.bold: true
            color: KHangMan.letterColor
        }

        Label {
            id: lossCountLabel
            text: KHangMan.lossCount
            font.pixelSize: 40
            font.bold: true
            color: KHangMan.letterColor
        }
    }

    Kirigami.Icon {
        id: successImage

        source: "data-success"
        visible: false

        width: Kirigami.Units.iconSizes.huge
        height: Kirigami.Units.iconSizes.huge

        anchors {
            horizontalCenter: parent.horizontalCenter;
            verticalCenter: parent.verticalCenter;
            verticalCenterOffset: -parent.height/4;
        }
    }

    Image {
        id: gallowsSeriesImage;
        source: gallowsSeriesCounter == 0 ? "" : "qrc:/qml/gallows/gallows" + gallowsSeriesCounter + ".png"
        visible: (isPlaying && gallowsSeriesCounter > 0)

        anchors {
            horizontalCenter: parent.horizontalCenter;
            verticalCenter: parent.verticalCenter;
            verticalCenterOffset: -parent.height/4;
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
            model: KHangMan.currentWord;
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
            bottom: parent.bottom;
            bottomMargin: 10;
        }

        spacing: gamePage.width/35;
        columns: 13;
        Repeater {
            id: alphabetLetterRepeater;
            model: alphabet;
            Button {
                id: alphabetButton;
                property string letter: modelData
                property string upperCase: modelData.toUpperCase()
                property string buttonColor: "black"

                background: Kirigami.ShadowedRectangle {
                    id: alphabetLetterIdStyleRectangle
                    implicitWidth: gamePage.width / 22
                    implicitHeight: gamePage.width / 22
                    color: buttonColor
                    radius: 8
                    shadow {
                        size: 4
                        xOffset: 3
                        yOffset: 3
                        color: Qt.rgba(0, 0, 0, 0.5)
                    }
                }

                contentItem: Text {
                    id: buttonLabel
                    anchors.centerIn: parent
                    text: letter
                    font.family : "Arial"
                    font.pixelSize: gamePage.width / 40
                    font.capitalization : Font.AllUppercase
                    font.weight : Font.Bold
                    horizontalAlignment : Text.AlignHCenter
                    verticalAlignment : Text.AlignVCenter
                    color: parent.enabled ? "white" : "grey"
                }

                onClicked: {
                    guessLetter(modelData);
                }
            }
        }
    }

    Label {
        id: hintLabel
        text: KHangMan.currentHint
        font.family: "serif-sans"
        color: "green"
        font.italic: true
        font.pixelSize: gamePage.width / 60
        anchors.top: currentWordGrid.bottom
        anchors.bottom: alphabetGrid.top
        anchors.horizontalCenter: parent.horizontalCenter
        visible: false
    }

    footer: ToolBar {
        id: mainPageTools
        visible: isPlaying

        RowLayout {
            anchors.fill: parent

            ToolButton {
                id: helpHintButton
                icon.source: "Images/help-hint.png"
                ToolTip.text: i18n("Display the hint.")
                ToolTip.visible: hovered
                ToolTip.delay: Kirigami.Units.toolTipDelay
                enabled: hintLabel.text != ""

                onClicked: {
                    // make the button toggle between display and hide the hint
                    hintLabel.visible = hintLabel.visible ? false : true
                }
            }

            ToolButton {
                id: categorySelectionButton
                Layout.fillWidth: true
                text: categorySelectionDialog.model[categorySelectionDialog.currentIndex];
                ToolTip.text: i18n("Change the category.")
                ToolTip.visible: hovered
                ToolTip.delay: Kirigami.Units.toolTipDelay

                onClicked: {
                    categorySelectionDialog.open();
                }
            }

            ToolButton {
                id: languageSelectionButton
                Layout.fillWidth: true
                text: languageSelectionDialog.model[languageSelectionDialog.currentIndex]
                ToolTip.text: i18n("Change the language.")
                ToolTip.visible: hovered
                ToolTip.delay: Kirigami.Units.toolTipDelay

                onClicked: {
                    languageSelectionDialog.open()
                }
            }

            ToolButton {
                id: revealWordButton
                Layout.fillWidth: true
                text: i18n("Reveal Word")
                ToolTip.text: i18n("Reveal the current word.")
                ToolTip.visible: hovered
                ToolTip.delay: Kirigami.Units.toolTipDelay

                onClicked: {
                    KHangMan.revealCurrentWord();
                    KHangMan.lossCount++;
                    if (KHangMan.soundEnabled) {
                        wrongSoundEffect.play();
                    }

                    khangmanResultTimer.start();
                }
            }

            Text {
                id: timerText
                visible: KHangMan.resolveTime === 0 ? false : true
                text: Math.floor(countDownTimerValue / 60) + ":" + Math.floor(countDownTimerValue % 60 / 10)
                      + Math.floor(countDownTimerValue % 60 % 10)
            }

            ToolButton {
                id: nextWordButton
                Layout.fillWidth: true
                icon.name: "go-next"
                ToolTip.text: i18n("Load the next word and start a new game.")
                ToolTip.visible: hovered
                ToolTip.delay: Kirigami.Units.toolTipDelay

                onClicked: {
                    if (KHangMan.soundEnabled) {
                        nextWordSoundEffect.play();
                    }

                    nextWord();

                    secondTimer.repeat = true;
                    secondTimer.restart();
                }
            }
        }
    }

    Keys.onPressed: (event) => {
        if (event.text.length > 0) {
            guessLetter(event.text);
        }
    }
}
