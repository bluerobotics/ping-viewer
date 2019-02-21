import QtGraphicalEffects 1.0
import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

import Flasher 1.0
import SettingsManager 1.0

Item {
    id: root
    height: childrenRect.height
    width: childrenRect.width
    property var ping: null
    property var running: false

    Connections {
        target: ping.flasher
        onFlashProgress: {
            running = true
            flashProgress.value = progress
        }
        onMessageChanged: {
            switch(state) {
                case Flasher.Error:
                    print("Flash procedure failed!")
                    print("Error: ", ping.flasher.message)

                case Flasher.FlashFinished:
                    running = false
                    flashProgress.value = 0.0
                    break;
                case Flasher.Flashing:
                case Flasher.StartingFlash:
                    running = true
                    break;
                default:
                    running = true
            }
        }
    }

    GroupBox {
        id: firmwareGroup
        title: "Firmware Update"
        enabled: true
        // Hack
        label.x: width/2 - label.contentWidth/2
        width: 600

        ColumnLayout {
            anchors.fill: parent

            RowLayout {
                Layout.fillWidth: true

                ComboBox {
                    id: automaticUpdateCB
                    model: ["Automatic Update", "Manual Update"]
                    Layout.fillWidth: true
                }

                ComboBox {
                    id: baudComboBox
                    // This should use the same values in Flasher::_validBaudRates
                    model: [57600, 115200, 230400]
                    Layout.fillWidth: true
                    visible: SettingsManager.debugMode
                }

                CheckBox {
                    id: verifyCB
                    text: "Verify"
                    visible: SettingsManager.debugMode
                    checked: true
                }

                CheckBox {
                    id: bootLoaderCB
                    text: "Send reset"
                    visible: SettingsManager.debugMode
                    checked: true
                }
            }

            RowLayout {
                Layout.fillWidth: true

                Label {
                    text: "Current Firmware: " + ping.firmware_version_major + "." + ping.firmware_version_minor
                }

                Label {
                    id: firmwareLabel
                    text: "Firmware File: " + fileDialog.fileName
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight
                    visible: automaticUpdateCB.currentIndex === 1
                }

                Label {
                    text: "Latest version: "
                    visible: automaticUpdateCB.currentIndex === 0
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignRight
                }

                ComboBox {
                    id: fwCombo
                    visible: automaticUpdateCB.currentIndex === 0
                    model: Object.keys(ping.firmwaresAvailable)
                    Layout.minimumWidth: 220
                }

                PingButton {
                    id: browseBt
                    text: "Browse.."
                    enabled: !running
                    visible: automaticUpdateCB.currentIndex === 1
                    onClicked: {
                        fileDialog.visible = true
                    }
                }
            }

            PingButton {
                text: "Firmware Update"
                Layout.fillWidth: true
                enabled: !running && ((fwCombo.currentText != "" && !browseBt.visible) || (fileDialog.fileName != "" && browseBt.visible))

                onClicked: {
                    var baud = SettingsManager.debugMode ? baudComboBox.model[baudComboBox.currentIndex] : 57600
                    var verify = SettingsManager.debugMode ? verifyCB.checked : true
                    var path = automaticUpdateCB.currentIndex ? fileDialog.fileUrl : ping.firmwaresAvailable[fwCombo.currentText]
                    running = true
                    ping.firmwareUpdate(path, bootLoaderCB.checked, baud, verifyCB.checked)
                }
            }

            ProgressBar {
                id: flashProgress
                indeterminate: !running
                Layout.fillWidth: true
                value: 0.0
                from: 0.0
                to: 100.0
            }

            Label {
                text: "Error:"
                font.bold: true
                visible: ping.flasher.message !== ""
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }

            Label {
                text: ping.flasher.message
                visible: ping.flasher.message !== ""
                Layout.fillWidth: true
            }
        }
    }

    PingFileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        visible: false
        nameFilters: ["Hex Files (*.hex)"]
    }
}
