// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.components as Components

QQC2.Dialog {
    id: root

    property alias model: selectionListView.model
    property alias delegate: selectionListView.delegate
    property alias currentIndex: selectionListView.currentIndex

    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)
    z: Kirigami.OverlayZStacking.z

    width: 400
    height: 200

    leftPadding: 0
    topPadding: 0
    rightPadding: 0
    bottomPadding: 0

    modal: true

    parent: applicationWindow().QQC2.Overlay.overlay

    background: Components.DialogRoundedBackground {}

    header: ColumnLayout {
        width: parent.width
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.smallSpacing
            spacing: Kirigami.Units.smallSpacing

            Kirigami.Heading {
                text: root.title
                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.smallSpacing
                Layout.rightMargin: Kirigami.Units.smallSpacing
            }

            QQC2.ToolButton {
                icon.name: "dialog-close"
                text: i18nc("@action:button", "Close")
                display: QQC2.Button.IconOnly
                onClicked: root.close();
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }
    }

    contentItem: ColumnLayout {
        ListView {
            id: selectionListView

            Layout.fillWidth: true
            Layout.fillHeight: true

            Kirigami.Theme.colorSet: Kirigami.Theme.View

            currentIndex : -1
            focus: true
            clip: true
        }
    }
}


