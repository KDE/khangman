// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// Copyright (C) 2014 Rahul Chowdhury <rahul.chowdhury@kdemail.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Window
import org.kde.khangman

Dialog {
    id: mainSettingsDialog

    signal okClicked()
    signal cancelClicked()

    title: i18n("KHangMan Settings")

    contentItem: Rectangle {
        id: mainSettingsRectangle
        color: "black"
        implicitWidth: 600
        implicitHeight: 230

        property int settingsPageMargins: 15;

        MessageDialog {
            id: khangmanWordResolveTimeUserGuideDialog;
            title: i18n("KHangMan word resolve time")
            text: i18n("Maximum allowed time for resolving the word in KHangMan")
            buttons: MessageDialog.Ok
        }

        MessageDialog {
            id: hintShowDurationUserGuideDialog;
            title: i18n("Hint show duration")
            text: i18n("The duration for showing the hint for the actual word");
            buttons: MessageDialog.Ok
        }
        MessageDialog {
            id: scoreMultiplyingFactorUserGuideDialog;
            title: i18n("KHangMan Score Multiplying Factor")
            text: i18n("Determine the factor by which the scores will be multiplied.");
            buttons: MessageDialog.Ok
        }

        MessageDialog {
            id: soundsUserGuideDialog;
            title: i18n("Sounds")
            text: i18n("Turn all the sounds on or off inside the game");
            buttons: MessageDialog.Ok
        }

        function saveSettings() {
            KHangMan.resolveTime = resolveTimeSlider.value
            KHangMan.soundEnabled = soundsSwitch.checked
            KHangMan.scoreMultiplyingFactor = parseInt(scoreMultiplyingFactorTextEdit.text)
        }

        function resetSettings () {
            resolveTimeSlider.value = KHangMan.resolveTime
            soundsSwitch.checked = KHangMan.soundEnabled
            scoreMultiplyingFactorTextEdit.text = KHangMan.scoreMultiplyingFactor
        }

        Component.onCompleted: {
            resolveTimeSlider.value = KHangMan.resolveTime;
            soundsSwitch.checked = KHangMan.soundEnabled;
        }


        ColumnLayout {
            id: settingsPageMainColumn;
            width: parent.width

            spacing: 10;

            Label {
                id: settingsPageHeadingLabel
                text: i18n("KHangMan Settings");
                Layout.margins: 5
                font.pixelSize: 32;
                font.bold: true
                color: "white"
            }

            RowLayout {
                width: parent.width;
                spacing: 10
                id: resolveTimeRowLayout

                Label {
                    id: resolveTimeLabel;

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                    Layout.margins: 5

                    text: i18n("Word resolve time in seconds");
                    font.bold: true
                    color: "white"
                }

                Slider {
                    id: resolveTimeSlider;
                    stepSize: 15;
                    from: 0;
                    to: 300;
                    Layout.fillWidth: true
                }

                Label {
                    id: resolveTimeSliderCurrentValue
                    text: i18n(resolveTimeSlider.value + (resolveTimeSlider.value == 1 ? " second" : " seconds"))
                    color: "white"
                    font.bold: true
                }

                ToolButton {
                    icon.source: "Images/dialog-information.png"

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                    onClicked: {
                        khangmanWordResolveTimeUserGuideDialog.open();
                    }
                }
            }

            RowLayout {
                id: scoreMultiplyingFactorLayout
                width: parent.width

                Label {
                    id: scoreMultiplyingFactorLabel
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                    Layout.margins: 5

                    text: i18n("Score Multiplying Factor")
                    font.bold: true
                    color: "white"
                }

                Rectangle {
                    id: scoreMultiplyingFactorInputBox
                    color: "black"
                    border.color: "white"
                    height: scoreMultiplyingFactorLabel.height
                    width: height * 5

                    TextEdit {
                        id: scoreMultiplyingFactorTextEdit
                        horizontalAlignment: TextEdit.AlignJustify
                        wrapMode: TextEdit.Wrap
                        text: KHangMan.scoreMultiplyingFactor.toString()
                        font.bold: true
                        color: "white"
                        focus: true
                    }
                }
                    ToolButton {
                        icon.source: "Images/dialog-information.png"

                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                        onClicked: {
                            scoreMultiplyingFactorUserGuideDialog.open();
                    }
                }
            }

            RowLayout {
                width: parent.width;
                id: soundRowLayout

                Label {
                    id: soundLabel
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                    Layout.margins: 5

                    text: i18n("Sounds");
                    font.bold: true
                    color: "white"
                }

                CheckBox {
                    id: soundsSwitch;
                    Layout.alignment: Qt.AlignVCenter
                }

                ToolButton {
                    icon.source: "Images/dialog-information.png"

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                    onClicked: {
                        soundsUserGuideDialog.open();
                    }
                }
            }

            RowLayout {
                width: parent.width;
                id: buttonLayout
                spacing: 0
                Layout.alignment: Qt.AlignRight

                Button {
                    id: okButton

                    background: Rectangle {
                        id: okButtonStyleRectangle
                        color: okButton.pressed ? "grey" : "white"
                        radius: 8
                    }

                    contentItem: Label {
                        id: okButtonLabel
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                        text: i18n("OK")
                        font.family : "Arial"
                        font.capitalization : Font.AllUppercase
                        font.weight : Font.Bold
                        horizontalAlignment : Text.AlignHCenter
                        verticalAlignment : Text.AlignVCenter
                        color: "black"
                    }

                    onClicked: {
                        // save the current settings
                        mainSettingsRectangle.saveSettings()
                        mainSettingsDialog.okClicked()
                    }
                }

                Button {
                    id: cancelButton

                    Layout.margins: 5

                    background: Rectangle {
                        id: cancelButtonStyleRectangle
                        height: 10
                        width: 50
                        color: cancelButton.pressed ? "grey" : "white"
                        radius: 8
                    }

                    contentItem: Label {
                        id: cancelButtonLabel
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                        text: i18n("Cancel")
                        font.family : "Arial"
                        font.capitalization : Font.AllUppercase
                        font.weight : Font.Bold
                        horizontalAlignment : Text.AlignHCenter
                        verticalAlignment : Text.AlignVCenter
                        color: "black"
                    }

                    onClicked: {
                        // ignore the changes made to the settings
                        mainSettingsRectangle.resetSettings()
                        mainSettingsDialog.cancelClicked()
                    }
                }

            }
        }
    }
}
