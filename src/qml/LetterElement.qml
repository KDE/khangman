// SPDX-FileCopyrightText: 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-FileCopyrightText: 2014 Rahul Chowdhury <rahul.chowdhury@kdemail.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import org.kde.khangman

Rectangle {
    property string letterText: "";
    property int letterPixelSize: 40;

    color: Qt.rgba(0, 0, 0, 0);
    width: 48;
    height: 48;

    Text {
        id: letterId;
        text: letterText;
        color: KHangMan.letterColor;
        font {
            pixelSize: letterPixelSize;
            bold: true;
            capitalization: Font.AllUppercase;
        }

        anchors {
            horizontalCenter: parent.horizontalCenter;
            verticalCenter: parent.verticalCenter;
        }
    }
}

