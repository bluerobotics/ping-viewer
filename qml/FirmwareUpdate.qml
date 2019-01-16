import QtGraphicalEffects 1.0
import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

import SettingsManager 1.0

Item {
    id: firmwareUpdate
    height: settingsLayout.height
    width: settingsLayout.width
    property var ping: null

    Connections {
        target: ping
        onFlashProgress: {
            flashProgress.indeterminate = false;
            flashProgress.value = progress;
        }
        onFlashComplete: {
            console.log("flash complete!")
            flashProgress.value = 0.0;
            flashProgress.indeterminate = true;
        }
    }

    ColumnLayout {
        id: settingsLayout
        width: 600
        RowLayout {
            GroupBox {
                id: firmwareGroup
                title: "Firmware Update"
                enabled: true
                // Hack
                label.x: width/2 - label.contentWidth/2
                Layout.fillWidth: true

                GridLayout {
                    anchors.fill: parent
                    columns: 10
                    rowSpacing: 5
                    columnSpacing: 5

                    Text {
                        text: "Current Firmware:"
                        color: Material.primary
                    }

                    PingTextField {
                        enabled: false
                        Layout.columnSpan: SettingsManager.debugMode ? 3 : 9
                        Layout.fillWidth: true
                        text: ping.firmware_version_major + "." + ping.firmware_version_minor
                    }

                    ComboBox {
                        id: baudComboBox
                        // This should use the same values in Ping::FlashBaudrate
                        model: [57600, 115200, 230400]
                        Layout.columnSpan: 3
                        Layout.fillWidth: true
                        visible: SettingsManager.debugMode
                    }

                    CheckBox {
                        id: verifyCB
                        text: "Verify"
                        visible: SettingsManager.debugMode
                        Layout.columnSpan: 2
                        checked: true
                    }

                    CheckBox {
                        id: bootLoaderCB
                        text: "Send reset"
                        visible: SettingsManager.debugMode
                        Layout.columnSpan: 1
                        checked: true
                    }

                    Text {
                        id: firmwareLabel
                        text: "Firmware File:"
                        color: Material.primary
                    }

                    PingTextField {
                        id: firmwareFileName
                        enabled: false
                        Layout.columnSpan:  8
                        Layout.fillWidth: true
                        text: fileDialog.fileName
                    }

                    PingButton {
                        text: "Browse.."
                        enabled: flashProgress.indeterminate
                        onClicked: {
                            fileDialog.visible = true
                        }
                    }

                    PingButton {
                        text: "Firmware Update"
                        Layout.columnSpan:  10
                        Layout.fillWidth: true
                        enabled: fileDialog.fileUrl != undefined && flashProgress.indeterminate

                        onClicked: {
                            var baud = SettingsManager.debugMode ? baudComboBox.model[baudComboBox.currentIndex] : 57600
                            var verify = SettingsManager.debugMode ? verifyCB.checked : true
                            ping.firmwareUpdate(fileDialog.fileUrl, bootLoaderCB.checked, baud, verifyCB.checked)
                        }
                    }

                    ProgressBar {
                        id: flashProgress
                        indeterminate: true
                        Layout.columnSpan: 10
                        Layout.fillWidth: true
                        value: 0.0
                        from: 0.0
                        to: 100.0
                    }
                }
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
