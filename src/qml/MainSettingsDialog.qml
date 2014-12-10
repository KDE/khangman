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
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
//import com.nokia.meego 1.0
//import com.nokia.extras 1.0

Dialog {
    id: mainSettingsDialog

    title: i18n("KHangMan Settings")

    contentItem: Rectangle {
        id: mainSettingsRectangle
        color: "black"
        implicitWidth: 600
        implicitHeight: 200

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
            id: soundsUserGuideDialog;
            title: i18n("Sounds")
            text: i18n("Turn all the sounds on or off inside the game");
            icon: StandardIcon.Information
            standardButtons: StandardButton.Ok
        }

        function pushPage(file) {
            var component = Qt.createComponent(file)
            if (component.status == Component.Ready)
                pageStack.push(component);
            else
                console.log(i18n("Error loading component:", component.errorString()));
        }

        MySelectionDialog {
            id: languageSelectionDialog;
            title: i18n("Select a language");
            selectedIndex: 0;

            model: khangman.languageNames();

            onSelectedIndexChanged: {
                khangman.selectedLanguage = model[selectedIndex];
            }
        }

        Connections {
            target: khangman;

            onHintHideTimeChanged: {
                hintAppearanceSlider.value = khangman.hintHideTime;
            }

            onResolveTimeChanged: {
                resolveTimeSlider.value = khangman.resolveTime;
            }

            onSoundChanged: {
                soundsSwitch.checked = khangman.sound;
            }
        }

        Component.onCompleted: {
            hintAppearanceSlider.value = khangman.hintHideTime;
            resolveTimeSlider.value = khangman.resolveTime;
            soundsSwitch.checked = khangman.sound;
        }


        ColumnLayout {
            id: settingsPageMainColumn;
            width: parent.width

            spacing: 10;

            Label {
                id: settingsPageHeadingLabel
                // width: parent.width;
                text: i18n("KHangMan Settings");
                font.pixelSize: 32;
                font.bold: true
                color: "white"
            }

            /*Image {
                id: separator1;
                width: parent.width;
                height: 2;
                fillMode: Image.TileHorizontally;
                source: "separator.png";
            }*/

            RowLayout {
                id: hintAppearanceRowLayout
                height: 50
                Layout.fillWidth: true
                //width: parent.width;
                spacing: 10

                Label {
                    id: hintAppearanceLabel;

                    /*anchors {
                        left: parent.left;
                        verticalCenter: parent.verticalCenter;
                    }*/

                    text: i18n("Hint show duration in seconds");
                    font.bold: true
                    color: "white"
                    Layout.alignment: Qt.AlignLeft
                }

                Slider {
                    id: hintAppearanceSlider;
                    width: parent.width - 10;
                    stepSize: 1;
                    tickmarksEnabled : true
                    updateValueWhileDragging : true
                    //valueIndicatorVisible: true;
                    minimumValue: 0;
                    maximumValue: 10;
                    //anchors.verticalCenter: parent.verticalCenter
                    Layout.alignment: Qt.AlignHCenter

                    onValueChanged: {
                        khangman.hintHideTime = value;
                    }
                }

                Label {
                    id: hintAppearanceSliderCurrentValue
                    text: i18n(hintAppearanceSlider.value + (hintAppearanceSlider.value == 1 ? " second" : " seconds"))
                    color: "white"
                    font.bold: true
                }

                ToolButton {
                    //iconSource: "image://theme/icon-l-user-guide-main-view"
                    id: hintAppearanceToolButton
                    iconSource: "dialog-information.png"

                    /*anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter;
                    }*/
                    Layout.alignment: Qt.AlignRight

                    onClicked: {
                        hintShowDurationUserGuideDialog.open();
                    }
                }
            }

            /*Image {
                id: separator2;
                width: parent.width;
                height: 2;
                fillMode: Image.TileHorizontally;
                source: "separator.png";
            }*/

            RowLayout {
                //height: childrenRect.height;
                // height: 100
                width: parent.width;
                spacing: 10
                id: resolveTimeRowLayout
                anchors.top: hintAppearanceRowLayout.bottom

                Label {
                    id: resolveTimeLabel;

                    anchors {
                        left: parent.left;
                        verticalCenter: parent.verticalCenter;
                    }

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
                    //valueIndicatorVisible: true;
                    minimumValue: 0;
                    maximumValue: 300;
                    //anchors.left: resolveTimeLabel.right
                    anchors.verticalCenter: parent.verticalCenter

                    onValueChanged: {
                        khangman.resolveTime = value;
                    }
                }

                Label {
                    id: resolveTimeSliderCurrentValue
                    text: i18n(resolveTimeSlider.value + (resolveTimeSlider.value == 1 ? " second" : " seconds"))
                    color: "white"
                    font.bold: true
                }

                ToolButton {
                    //iconSource: "image://theme/icon-l-user-guide-main-view"
                    iconSource: "dialog-information.png"

                    anchors {
                        //left: resolveTimeSlider.right;
                        //verticalCenter: parent.verticalCenter;
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }

                    onClicked: {
                        khangmanWordResolveTimeUserGuideDialog.open();
                    }
                }
            }

            /*Image {
                id: separator3;
                width: parent.width;
                height: 2;
                fillMode: Image.TileHorizontally;
                source: "separator.png";
            }*/

            RowLayout {
                //height: childrenRect.height;
                // height: 150
                width: parent.width;
                id: soundRowLayout

                Label {
                    id: soundLabel
                    anchors {
                        left: parent.left;
                        verticalCenter: parent.verticalCenter;
                    }

                    text: i18n("Sounds");
                    font.bold: true
                    color: "white"
                }

                CheckBox {
                    id: soundsSwitch;
                    anchors {
                        verticalCenter: parent.verticalCenter;
                    }
                    checked: khangman.sound

                    onClicked: {
                        khangman.sound = checked
                    }
                }

                ToolButton {
                    //iconSource: "image://theme/icon-l-user-guide-main-view"
                    iconSource: "dialog-information.png"

                    anchors {
                        right: parent.right;
                        verticalCenter: parent.verticalCenter;
                    }

                    onClicked: {
                        soundsUserGuideDialog.open();
                    }
                }
            }

            /*Image {
                id: separator4;
                width: parent.width;
                height: 2;
                fillMode: Image.TileHorizontally;
                source: "separator.png";
            }*/

            RowLayout {
                id: languageRowLayout
                width: parent.width
                anchors.top: soundRowLayout.bottom

                Label {
                    id: languageSelectionLabel
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                    }
                    text: i18n("Language")
                    font.bold: true
                    color: "white"
                }

                ToolButton {
                    id: languageSelectionToolButton
                    iconSource: "language-settings.png"

                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }

                    tooltip: i18n("Click here to select your preferred language")

                    onClicked: {
                        languageSelectionDialog.open()
                    }
                }

            }
        }
    }
}