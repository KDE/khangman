/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Components project.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls
import QtQuick.Controls.Styles 1.2

Dialog {
    id: root
    width: 400 // Set a width and height so title bar title will show up properly
    height: 160

    // Common API
    property alias model: selectionListView.model
    property int selectedIndex: -1   // read & write

    property Component delegate:          // Note that this is the default delegate for the list
        Component {
            id: defaultDelegate

            Item {
                id: delegateItem
                property bool selected: index == selectedIndex;

                height: categoryButton.height * 1.2
                anchors.left: root.left
                anchors.right: root.right
                anchors.leftMargin: categoryButton.height * 1.2

                Button {
                    id: categoryButton

                    style: ButtonStyle {
                        id: categoryButtonStyle
                        background: Rectangle {
                            id: backgroundRect
                            anchors.fill: parent
                            color: "black"
                            radius: 8
                        }
                        label: Text {
                            id: itemText
                            elide: Text.ElideRight
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.fill: parent
                            font.family : "Arial"
                            font.weight : Font.Bold
                            horizontalAlignment : Text.AlignHCenter
                            verticalAlignment : Text.AlignVCenter
                            text: modelData
                            color: "white"
                        }
                    }

                    onClicked: {
                        selectedIndex = index;
                        accept();
                    }
                }
                Component.onCompleted: {
                    try {
                        // Legacy. "name" used to be the role which was used by delegate
                        itemText.text = name
                    } catch(err) {
                        try {
                            // "modelData" available for JS array and for models with one role
                            itemText.text = modelData
                        } catch (err) {
                            try {
                                 // C++ models have "display" role available always
                                itemText.text = display
                            } catch(err) {

                            }
                        }
                    }
                }
            }
        }

        title: "Selection Dialog"

        // the content field which contains the selection content
        contentItem: Item {

        id: selectionContent
        property int listViewHeight
        width: root.width

        ListView {
            id: selectionListView
            model: ListModel {}

            currentIndex : -1
            anchors.fill: parent
            delegate: root.delegate
            focus: true
            clip: true
        }
    }
}


