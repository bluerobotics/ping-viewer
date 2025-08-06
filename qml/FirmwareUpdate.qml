import DeviceManager 1.0
import Flasher 1.0
import Qt5Compat.GraphicalEffects
import QtQml 2.2
import QtQuick 2.15
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs
import QtQuick.Layouts 1.3
import SettingsManager 1.0

RowLayout {
    id: root

    property var ping: DeviceManager.primarySensor
    property var running: false

    Connections {
        function onFlashProgress(progress) {
            running = true;
            flashProgress.value = progress;
        }

        function onStateChanged(state) {
            switch (state) {
            case Flasher.Error:
                print("Flash procedure failed!");
                print("Error: ", ping.flasher.message);
            case Flasher.FlashFinished:
                running = false;
                flashProgress.value = 0;
                break;
            case Flasher.Flashing:
            case Flasher.StartingFlash:
                running = true;
                break;
            default:
                running = true;
            }
        }

        target: ping ? ping.flasher : null
    }

    PingGroupBox {
        id: firmwareGroup

        title: "Firmware Update"
        enabled: true
        Layout.fillWidth: true

        ColumnLayout {
            anchors.fill: parent

            RowLayout {
                Layout.fillWidth: true

                PingComboBox {
                    id: automaticUpdateCB

                    model: ["Automatic Update", "Manual Update"]
                    Layout.fillWidth: true
                }

                Label {
                    text: "Baudrate:"
                    visible: baudComboBox.visible
                }

                PingComboBox {
                    id: baudComboBox

                    model: ping.flasher.validBaudRates
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
                    property string version: ping ? ping.firmware_version_major + "." + ping.firmware_version_minor : "-"

                    text: "Current Firmware: " + version
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

                PingComboBox {
                    id: fwCombo

                    function sortPingFiles(firmwares) {
                        return Object.keys(firmwares).sort((first, second) => {
                            // Extract device type and version
                            const regex = /^(Ping|Ping360|Ping2)[-_]V(\d+)\.(\d+)(?:\.(\d+))?/i;
                            const firstMatch = first.match(regex);
                            const secondMatch = second.match(regex);
                            if (!firstMatch || !secondMatch)
                                return 0;

                            // Compare device types first
                            const firstDevice = firstMatch[1];
                            const secondDevice = secondMatch[1];
                            if (firstDevice !== secondDevice)
                                return firstDevice.localeCompare(secondDevice);

                            // Compare major version
                            const firstMajor = parseInt(firstMatch[2]);
                            const secondMajor = parseInt(secondMatch[2]);
                            if (firstMajor !== secondMajor)
                                return secondMajor - firstMajor;

                            // Compare minor version
                            const firstMinor = parseInt(firstMatch[3]);
                            const secondMinor = parseInt(secondMatch[3]);
                            if (firstMinor !== secondMinor)
                                return secondMinor - firstMinor;

                            // Compare patch version if available
                            const firstPatch = firstMatch[4] ? parseInt(firstMatch[4]) : 0;
                            const secondPatch = secondMatch[4] ? parseInt(secondMatch[4]) : 0;
                            return secondPatch - firstPatch;
                        });
                    }

                    visible: automaticUpdateCB.currentIndex === 0
                    model: ping && ping.firmwaresAvailable ? sortPingFiles(ping.firmwaresAvailable) : []
                    Layout.minimumWidth: 220
                }

                PingButton {
                    id: browseBt

                    text: "Browse.."
                    enabled: !running
                    visible: automaticUpdateCB.currentIndex === 1
                    onClicked: {
                        fileDialog.visible = true;
                    }
                }

            }

            PingButton {
                text: "Firmware Update"
                Layout.fillWidth: true
                enabled: !running && ((fwCombo.currentText != "" && !browseBt.visible) || (fileDialog.fileName != "" && browseBt.visible))
                onClicked: {
                    var baud = SettingsManager.debugMode ? baudComboBox.model[baudComboBox.currentIndex] : 115200;
                    var verify = SettingsManager.debugMode ? verifyCB.checked : true;
                    var path = automaticUpdateCB.currentIndex ? fileDialog.fileUrl : ping.firmwaresAvailable[fwCombo.currentText];
                    running = true;
                    ping.firmwareUpdate(path, bootLoaderCB.checked, baud, verifyCB.checked);
                }
            }

            ProgressBar {
                id: flashProgress

                indeterminate: !running
                Layout.fillWidth: true
                value: 0
                from: 0
                to: 100
            }

            Label {
                text: "Error:"
                font.bold: true
                visible: ping && ping.flasher.message !== ""
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }

            Label {
                text: ping ? ping.flasher.message : ""
                visible: ping ? ping.flasher.message !== "" : ""
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
