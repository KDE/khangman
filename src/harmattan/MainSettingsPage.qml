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
import com.nokia.meego 1.0
import com.nokia.extras 1.0

Page {

    property int settingsPageMargins: 15;
    function pushPage(file) {
        var component = Qt.createComponent(file)
        if (component.status == Component.Ready)
            pageStack.push(component);
        else
            console.log("Error loading component:", component.errorString());
    }

    MySelectionDialog {
        id: languageSelectionDialog;
        titleText: qsTr("Select a language");
        selectedIndex: 0;

        model: khangmanEngineHelper.languageNames();

        onSelectedIndexChanged: {
            khangmanEngineHelper.selectedLanguage = model[selectedIndex];
        }
    }

    Connections {
        target: khangmanEngineHelper;

        onHintHideTimeChanged: {
            hintAppearanceSlider.value = khangmanEngineHelper.hintHideTime;
        }

        onResolveTimeChanged: {
            resolveTimeSlider.value = khangmanEngineHelper.resolveTime;
        }

        onSoundChanged: {
            soundsSwitch.checked = khangmanEngineHelper.sound;
        }
    }

    Component.onCompleted: {
        hintAppearanceSlider.value = khangmanEngineHelper.hintHideTime;
        resolveTimeSlider.value = khangmanEngineHelper.resolveTime;
        soundsSwitch.checked = khangmanEngineHelper.sound;
    }


    Rectangle {
        id: settingsPageMainRectangle;
        color: "black";
        anchors.fill: parent;

        Flickable {
            anchors {
                fill: parent;
                margins: settingsPageMargins;
            }

            contentWidth: settingsPageMainColumn.width;
            contentHeight: settingsPageMainColumn.height;

            Column {
                id: settingsPageMainColumn;
                width: settingsPageMainRectangle.width - 2*settingsPageMargins;
                anchors {
                    margins: 5;
                }

                spacing: 10;

                Label {
                    width: parent.width;
                    text: qsTr("KHangMan Settings");
                    font.pixelSize: 32;
                }

                Image {
                    id: separator1;
                    width: parent.width;
                    height: 2;
                    fillMode: Image.TileHorizontally;
                    source: "separator.png";
                }

                Column {
                    width: parent.width;
                    spacing: 5;

                    Label {
                        id: hintAppearanceLabel;
                        anchors.left: parent.left;
                        text: qsTr("Hint appearance in seconds");
                        font.bold: true;
                    }

                    Slider {
                        id: hintAppearanceSlider;
                        width: parent.width - 10;
                        stepSize: 1;
                        valueIndicatorVisible: true;
                        minimumValue: 0;
                        maximumValue: 10;
                        anchors.horizontalCenter: parent.horizontalCenter;

                        onValueChanged: {
                            khangmanEngineHelper.hintHideTime = value;
                        }
                    }
                }

                Image {
                    id: separator2;
                    width: parent.width;
                    height: 2;
                    fillMode: Image.TileHorizontally;
                    source: "separator.png";
                }

                Column {
                    width: parent.width;
                    spacing: 5;

                    Label {
                        id: resolveTimeLabel;
                        anchors.left: parent.left;
                        text: qsTr("Resolve time in seconds");
                        font.bold: true;
                    }

                    Slider {
                        id: resolveTimeSlider;
                        width: parent.width - 10;
                        stepSize: 15;
                        valueIndicatorVisible: true;
                        minimumValue: 0;
                        maximumValue: 300;
                        anchors.horizontalCenter: parent.horizontalCenter;

                        onValueChanged: {
                            khangmanEngineHelper.resolveTime = value;
                        }
                    }
                }

                Image {
                    id: separator3;
                    width: parent.width;
                    height: 2;
                    fillMode: Image.TileHorizontally;
                    source: "separator.png";
                }

                Item {
                    width: parent.width;
                    height: childrenRect.height;

                    Label {
                        anchors.left: parent.left;
                        text: qsTr("Sound");
                        font.bold: true;
                    }

                    Switch {
                        id: soundsSwitch;
                        anchors.right: parent.right;

                        onCheckedChanged: {
                            khangmanEngineHelper.sound = checked;
                        }
                    }
                }

                Image {
                    id: separator4;
                    width: parent.width;
                    height: 2;
                    fillMode: Image.TileHorizontally;
                    source: "separator.png";
                }

                ListItem {
                    iconSource: "preferences-desktop-locale.png";
                    titleText: qsTr("Language");
                    subtitleText: khangmanEngineHelper.selectedLanguage ? khangmanEngineHelper.selectedLanguage : "English";
                    iconId: "textinput-combobox-arrow";
                    iconVisible: true;
                    mousePressed: languageSelectionMouseArea.pressed;

                    MouseArea {
                        id: languageSelectionMouseArea;
                        anchors.fill: parent;
                        onClicked: {
                            languageSelectionDialog.open();
                       }
                    }
                }
            }
        }
    }

    tools: commonTools;
}
