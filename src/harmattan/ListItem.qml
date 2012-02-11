/******************************************************************************
 * This file is part of the Gluon Development Platform
 * Copyright (C) 2011 Laszlo Papp <lpapp@kde.org>
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

Rectangle {

    id: listItem;

    property string iconSource;
    property string titleText;
    property string subtitleText;

    property string iconId;
    property bool iconVisible: false;

    property bool mousePressed: false;

    height: 64;
    width: parent.width;

    color: "black";

    BorderImage {
        id: background;
        // Fill page orders
        anchors {
            fill: parent;
            leftMargin: -16;
            rightMargin: -16;
        }

        visible: mousePressed;
        source: theme.inverted ? "image://theme/meegotouch-panel-inverted-background-pressed" : "image://theme/meegotouch-panel-background-pressed";
    }

    Row {
        anchors.fill: parent;
        spacing: 16;

        Image {
            anchors.verticalCenter: parent.verticalCenter;
            visible: true;
            width: 48;
            height: 48;
            source: listItem.iconSource;
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter;

            Label {
                id: mainText
                text: listItem.titleText;
                font.family: "Nokia Pure Text";
                font.weight: Font.Bold;
                font.pixelSize: 26;
                color: mousePressed ? "#797979" : "#ffffff";
            }

            Label {
                id: subText
                text: listItem.subtitleText;
                font.family: "Nokia Pure Text Light";
                font.weight: Font.Normal;
                font.pixelSize: 22;
                color: mousePressed ? "#797979" : "#C8C8C8";

                visible: text != "";
            }
        }

    }

    Image {
        function handleIconId() {
            var prefix = "icon-m-";
            // check if id starts with prefix and use it as is
            // otherwise append prefix and use the inverted version if required
            if (iconId.indexOf(prefix) !== 0)
                iconId = prefix.concat(iconId).concat(theme.inverted ?  "-inverse" : "");

            return "image://theme/" + iconId;
        }

        visible: iconVisible;
        source: handleIconId();
        anchors {
            right: parent.right;
            verticalCenter: parent.verticalCenter;
        }
    }
}
