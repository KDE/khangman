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

Rectangle {
    property string letterText: "";
    property int letterPixelSize: 40;

    color: Qt.rgba(0, 0, 0, 0);
    width: 48;
    height: 48;

    Text {
        id: letterId;
        text: letterText;
        color: khangman.letterColor;
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

