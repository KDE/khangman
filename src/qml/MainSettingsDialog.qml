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
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2

Dialog {
    id: mainSettingsDialog

    signal okClicked()
    signal cancelClicked()

    title: i18n("KHangMan Settings")

    contentItem: Rectangle {
        id: mainSettingsRectangle
        color: "black"
        implicitWidth: 600
        implicitHeight: 210

        property int settingsPageMargins: 15;

        MessageDialog {
            id: khangmanWordResolveTimeUserGuideDialog;
            title: i18n("KHangMan word resolve time")
            text: i18n("Maximum allowed time for resolving the word in KHangMan")
            icon: StandardIcon.Information
            standardButtons: StandardButton.Ok
        }

        MessageDialog {
            id: hintShowDurationUserGuideDialog;
            title: i18n("Hint show duration")
            text: i18n("The duration for showing the hint for the actual word");
            icon: StandardIcon.Information
            standardButtons: StandardButton.Ok
        }
        MessageDialog {
            id: scoreMultiplyingFactorUserGuideDialog;
            title: i18n("KHangMan Score Multiplying Factor")
            text: i18n("Determine the factor by which the scores will be multiplied.");
            icon: StandardIcon.Information
            standardButtons: StandardButton.Ok
                    }

        MessageDialog {
            id: soundsUserGuideDialog;
            title: i18n("Sounds")
            text: i18n("Turn all the sounds on or off inside the game");
            icon: StandardIcon.Information
            standardButtons: StandardButton.Ok
        }

        function saveSettings() {
            khangman.resolveTime = resolveTimeSlider.value
            khangman.soundEnabled = soundsSwitch.checked
            khangman.scoreMultiplyingFactor = parseInt(scoreMultiplyingFactorTextEdit.text)
        }

        function resetSettings () {
            resolveTimeSlider.value = khangman.resolveTime
            soundsSwitch.checked = khangman.soundEnabled
            scoreMultiplyingFactorTextEdit.text = khangman.scoreMultiplyingFactor
        }

        Component.onCompleted: {
            resolveTimeSlider.value = khangman.resolveTime;
            soundsSwitch.checked = khangman.soundEnabled;
        }


        ColumnLayout {
            id: settingsPageMainColumn;
            width: parent.width

            spacing: 10;

            Label {
                id: settingsPageHeadingLabel
                text: i18n("KHangMan Settings");
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

                    text: i18n("Word resolve time in seconds");
                    font.bold: true
                    color: "white"
                }

                Slider {
                    id: resolveTimeSlider;
                    width: parent.width - 10;
                    stepSize: 15;
                    tickmarksEnabled : true
                    updateValueWhileDragging : true
                    minimumValue: 0;
                    maximumValue: 300;
                    Layout.alignment: Qt.AlignVCenter
                }

                Label {
                    id: resolveTimeSliderCurrentValue
                    text: i18n(resolveTimeSlider.value + (resolveTimeSlider.value == 1 ? " second" : " seconds"))
                    color: "white"
                    font.bold: true
                }

                ToolButton {
                    iconSource: "Images/dialog-information.png"

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
                        text: khangman.scoreMultiplyingFactor.toString()
                        font.bold: true
                        color: "white"
                        focus: true
                    }
                }
                    ToolButton {
                        iconSource: "Images/dialog-information.png"

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

                    text: i18n("Sounds");
                    font.bold: true
                    color: "white"
                }

                CheckBox {
                    id: soundsSwitch;
                    Layout.alignment: Qt.AlignVCenter
                }

                ToolButton {
                    iconSource: "Images/dialog-information.png"

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                    onClicked: {
                        soundsUserGuideDialog.open();
                    }
                }
            }

            Button {
                id: cancelButton

                Layout.alignment: Qt.AlignRight
                anchors {
                    rightMargin: 5
                }

                style: ButtonStyle {
                    background: Rectangle {
                        id: cancelButtonStyleRectangle
                        height: 10
                        width: 50
                        color: cancelButton.pressed ? "grey" : "white"
                        radius: 8
                    }

                    label: Text {
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
                }

                onClicked: {
                    // ignore the changes made to the settings
                    mainSettingsRectangle.resetSettings()
                    mainSettingsDialog.cancelClicked()
                }
            }

            Button {
                id: okButton

                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                anchors {
                    rightMargin: 10
                }

                style: ButtonStyle {
                    background: Rectangle {
                        id: okButtonStyleRectangle
                        height: 10
                        width: 50
                        color: okButton.pressed ? "grey" : "white"
                        radius: 8
                    }

                    label: Text {
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
                }

                onClicked: {
                    // save the current settings
                    mainSettingsRectangle.saveSettings()
                    mainSettingsDialog.okClicked()
                }
            }
        }
    }
}
