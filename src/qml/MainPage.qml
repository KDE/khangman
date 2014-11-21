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
import QtQuick.Dialogs 1.2
//import com.nokia.meego 1.0
///import com.nokia.extras 1.0

Item {

    Dialog {
        id: aboutDialog;
        title: i18n("About KHangMan")
        //icon: "hi80-app-khangman-harmattan.png";
        contentItem: Label {
            id: aboutDialogLabel
            text: i18n("KHangMan (Version 0.1.0)<br><br>(C) 2012 Laszlo Papp<br>lpapp@kde.org")
        }
        standardButtons: StandardButton.Ok
    }

    Column {
        id: mainPageColumn;

        anchors {
            centerIn: parent;
            verticalCenterOffset: -5;
        }

        width: 400;

        spacing: 20;

        Button {
            id: playResumeGameButton

            property string playResumeGameButtonLabel: i18n("Play Game");

            //font.pixelSize: 48;

            width: parent.width

            style: ButtonStyle {
                background: Rectangle {
                    anchors.fill: parent
                    color: "black"
                }
                label: Text {
                        anchors.centerIn: parent
                        horizontalAlignment: Text.AlignHCenter
                        text: playResumeGameButton.playResumeGameButtonLabel
                        color: "lightsteelblue"
                }
            }

            //iconSource: "play.png"

            onClicked: {
                rootWindow.push(gamePage)
                if( playResumeGameButton.playResumeGameButtonLabel == i18n("Play Game") ) {
                    gamePage.nextWord()
                    playResumeGameButton.playResumeGameButtonLabel = i18n("Resume Game")
                }
                gamePage.startTimer()
            }

            action: Action {
                id: playResumeGameButtonAction
                tooltip: playResumeGameButton.playResumeGameButtonLabel == i18n("Play Game") ? i18n("Start a new game.") : i18n("Resume your game.")
            }

            isDefault: true // selected by default, press Enter to trigger

        }

        Button {
            id: settingsButton;

            //font.pixelSize: 48;

            width: parent.width;

            onClicked: {
                rootWindow.push(mainSettingsPage);
            }

            //iconSource: "settings.png"

            style: ButtonStyle {
                background: Rectangle {
                    anchors.fill: parent
                    color: "black"
                }
                label: Text {
                    horizontalAlignment: Text.AlignHCenter
                    anchors.centerIn: parent
                    text: i18n("Settings");
                    color: "lightsteelblue"
                }
            }

            action: Action {
                id: settingsButtonAction
                tooltip: i18n("View and configure the Settings.")
            }
        }

        Button {
            id: aboutButton;

            anchors {
                horizontalCenter: parent.horizontalCenter;
            }

            text: i18n("About");
            //font.pixelSize: 48;

            width: parent.width;

            onClicked: {
                aboutDialog.open();
            }

            style: ButtonStyle {
                background: Rectangle {
                    anchors.fill: parent
                    color: "black"
                }
                label: Text {
                    horizontalAlignment: Text.AlignHCenter
                    anchors.centerIn: parent
                    text: i18n("About");
                    color: "lightsteelblue"
                }
            }
            //iconSource: "dialog-information.png"

            action: Action {
                id: aboutButtonAction
                tooltip: i18n("View details about the game.")
            }
        }
    }

    Image {
        id: kdeEduLogo;

        anchors {
            right: parent.right;
            bottom: parent.bottom;
            rightMargin: 5;
        }

        fillMode: Image.PreserveAspectFit;
        source: "kde-edu-logo.png";
    }
}