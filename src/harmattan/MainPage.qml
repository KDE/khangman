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

    QueryDialog {
        id: aboutDialog;
        icon: "hi80-app-khangman-harmattan.png";
        message: i18n("KHangMan (Version 0.1.0)<br><br>(C) 2012 Laszlo Papp<br>lpapp@kde.org");
        acceptButtonText: i18n("OK");
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
            id: playResumeGameButton;

            text: i18n("Play Game");
            font.pixelSize: 48;

            width: parent.width;

            onClicked: {
                pageStack.push(gamePage);
                text = i18n("Resume Game");
            }
        }

        Button {
            id: settingsButton;

            text: i18n("Settings");
            font.pixelSize: 48;

            width: parent.width;

            onClicked: {
                pageStack.push(mainSettingsPage);
            }
        }

        Button {
            id: helpButton;

            anchors {
                horizontalCenter: parent.horizontalCenter;
            }

            text: i18n("About");
            font.pixelSize: 48;

            width: parent.width;

            onClicked: {
                aboutDialog.open();
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
