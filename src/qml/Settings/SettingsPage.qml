// SPDX-FileCopyrightText: 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-FileCopyrightText: 2014 Rahul Chowdhury <rahul.chowdhury@kdemail.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Window
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.khangman

FormCard.FormCardPage {
    id: mainSettingsDialog

    signal okClicked()
    signal cancelClicked()

    title: i18n("KHangMan Settings")

    function saveSettings(): void {
        KHangMan.resolveTime = resolveTimeSlider.value;
        KHangMan.soundEnabled = soundsSwitch.checked;
        KHangMan.scoreMultiplyingFactor = scoreMultiplyingFactor.value;
    }

    function resetSettings(): void {
        resolveTimeSlider.value = KHangMan.resolveTime;
        soundsSwitch.checked = KHangMan.soundEnabled;
        scoreMultiplyingFactor.value = KHangMan.scoreMultiplyingFactor;
    }

    Component.onCompleted: {
        resolveTimeSlider.value = KHangMan.resolveTime;
        soundsSwitch.checked = KHangMan.soundEnabled;
    }

    topPadding: Kirigami.Units.gridUnit

    FormCard.FormCard {
        FormCard.AbstractFormDelegate {
            background: null
            contentItem: ColumnLayout {
                QQC2.Label {
                    id: resolveTimeLabel;
                    text: i18n("Word resolve time in seconds");
                    wrapMode: Text.WordWrap
                }

                RowLayout {
                    Layout.fillWidth: true
                    QQC2.Slider {
                        id: resolveTimeSlider;
                        stepSize: 15;
                        from: 0;
                        to: 300;
                        Layout.fillWidth: true
                    }

                    QQC2.Label {
                        text: i18n(resolveTimeSlider.value + (resolveTimeSlider.value == 1 ? " second" : " seconds"))
                        wrapMode: Text.WordWrap
                    }
                }

                QQC2.Label {
                    text: i18n("The duration for showing the hint for the actual word")
                    color: Kirigami.Theme.disabledTextColor
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormSpinBoxDelegate {
            id: scoreMultiplyingFactor

            label: i18n("Score Multiplying Factor:")
            value: KHangMan.scoreMultiplyingFactor

            contentItem.data: QQC2.Label {
                text: i18n("Determine the factor by which the scores will be multiplied.")
                color: Kirigami.Theme.disabledTextColor
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormSwitchDelegate {
            id: soundsSwitch
            text: i18nc("@option:check", "Enable sounds")
        }
    }

    footer: QQC2.ToolBar {
        contentItem: RowLayout {
            Item {
                Layout.fillWidth: true
            }

            QQC2.Button {
                text: i18nc("@action:button", "Save")
                icon.name: "document-save"
                onClicked: {
                    // save the current settings
                    mainSettingsRectangle.saveSettings()
                    mainSettingsDialog.okClicked()
                }
            }

            QQC2.Button {
                text: i18nc("@action:button", "Cancel")
                icon.name: "dialog-cancel"
                onClicked: {
                    // ignore the changes made to the settings
                    mainSettingsRectangle.resetSettings()
                    mainSettingsDialog.cancelClicked()
                }
            }
        }
    }
}
