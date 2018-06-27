import QtGraphicalEffects 1.0
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

import FileManager 1.0
import Ping 1.0
import Ping1DNamespace 1.0

Item {
    id: root
    visible: true
    focus: true

    Column {
        z: 1
        anchors.left: parent.left
        anchors.top: parent.top

        PingItem {
            id: menuContainer
            icon: "/icons/arrow_right_white.svg"
            item: MainMenu {
                ping: ping
            }

            onHideItemChanged: {
                if(hideItem == false) {
                    settingsMenu.hideItem = true
                    infoMenu.hideItem = true
                }
            }
        }

        PingItem {
            id: settingsMenu
            marginMult: 1
            icon: "/icons/settings_white.svg"
            item: ColumnLayout {
                spacing: 0
                PingItem {
                    id: displayItem
                    isSubItem: true
                    icon: "/icons/sun_white.svg"

                    item: DisplaySettings {
                        id: displaySettings
                        waterfallItem: ping1DVisualizer.waterfallItem
                    }

                    onHideItemChanged: {
                        if(hideItem == false) {
                            firmwareUpdate.hideItem = true
                        }
                    }
                }
                PingItem {
                    id: firmwareUpdate
                    isSubItem: true
                    icon: "/icons/chip_white.svg"

                    item: FirmwareUpdate {
                        ping: ping
                    }

                    onHideItemChanged: {
                        if(hideItem == false) {
                            displayItem.hideItem = true
                        }
                    }
                }
            }
            onHideItemChanged: {
                if(hideItem == false) {
                    menuContainer.hideItem = true
                    infoMenu.hideItem = true
                }
            }
        }

        PingItem {
            id: infoMenu
            icon: "/icons/info_white.svg"
            state: "fill-right"
            item: InfoPage {
                id: infoPage
                anchors.fill: parent
                deviceFirmware: ping.fw_version_major + "." + ping.fw_version_minor
                deviceID: ping.srcId
                deviceModel: ping.device_model
                deviceType: ping.device_type
            }
            onHideItemChanged: {
                if(hideItem == false) {
                    menuContainer.hideItem = true
                    settingsMenu.hideItem = true
                }
            }
        }
    }

    PingItem {
        id: replayMenu
        visible: displaySettings.replayItem
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        state: "bottom-left"
        spin: true
        icon: "/icons/disk_black.svg"
        item: RowLayout {
            PingButton {
                id: replayStartBt
                text: "▮▮"
                enabled: !ping.link.isOnline()
                onClicked: {
                    if(text == "▶") {
                        text = "▮▮"
                        ping.link.start()
                    } else {
                        text = "▶"
                        ping.link.pause()
                    }
                }
            }

            Text {
                id: timeText
                text: "Time:"
                color: Style.textColor
            }

            Slider {
                id: replaySlider
                enabled: !ping.link.isOnline()
                from: 0
                value: ping.link.packageIndex
                stepSize: 1
                to: ping.link.packageSize
                onValueChanged: {
                    if(ping.link.packageIndex !== value) {
                        ping.link.packageIndex = value
                        ping.link.start()
                    }
                }
            }

            Text {
                id: replayElapsed
                text: ping.link.isOnline() ? "00:00:00.000 / 00:00:00.000" : ping.link.elapsedTimeString + " / " + ping.link.totalTimeString
                color: Style.textColor
            }

            Text {
                id: replayFileName
                text: "File:"
                color: Style.textColor
            }

            PingButton {
                text: "Replay Data"
                onClicked: replayFileDialog.visible = true
            }
        }
    }

    FileDialog {
        id: replayFileDialog
        title: "Please choose a log file"
        folder: shortcuts.home
        nameFilters: ["Binary files (*.bin)"]
        onAccepted: {
            var sizeToRemove = replayFileDialog.folder.toString().length - replayFileDialog.fileUrl.toString().length + 1
            // 1 (File) : File (remove file://) : format
            var finalString
            var urlString = replayFileDialog.fileUrl.toString()
            if (urlString.startsWith("file:///")) {
                // Check if is a windows string (8) or linux (7)
                var sliceValue = urlString.charAt(9) === ':' ? 8 : 7
                finalString = urlString.substring(sliceValue)
            } else {
                finalString = urlString
            }
            ping.connectLink(["1", finalString, "r"])
            replayFileName.text = "File: " + replayFileDialog.fileUrl.toString().slice(sizeToRemove)
        }
    }

    PingRelease {
        id: pingRelease
        z: 2
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    Ping {
        id: ping

        onConnectionOpen: {
            firstRequest()
        }

        onLinkUpdate: {
            if(ping.link.isOpen()) {
                firstRequest()
            }
        }

        Component.onCompleted: {
            if(ping.link.isOpen()) {
                firstRequest()
            }
        }

        onPointsUpdate: {
            // Move from mm to m
            ping1DVisualizer.draw(ping.points, (ping.length_mm - ping.start_mm) * 1e-3, ping.confidence, ping.start_mm*1e-3, ping.distance*1e-3)
        }

        onDistanceUpdate: {
            ping1DVisualizer.setDepth(ping.distance/1e3)
        }

        onConfidenceUpdate: {
            ping1DVisualizer.setConfidence(ping.confidence)
        }

        function firstRequest() {
            ping.request(Ping1DNamespace.Mode)
            ping.request(Ping1DNamespace.Profile)
            ping.request(Ping1DNamespace.Fw_version)
            ping.request(Ping1DNamespace.Device_id)
            ping.request(Ping1DNamespace.Speed_of_sound)
            ping.pollFrequency = 15
        }
    }

    ColumnLayout {
        id: mainColumn
        anchors.fill: parent
        spacing: 0
        Ping1DVisualizer {
            id: ping1DVisualizer
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    PingStatus {
        ping: ping
        visible: displaySettings.debugMode
    }

    LinearGradient {
        anchors.fill: parent
        z: parent.z-1
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#FF11b3ff" }
            GradientStop { position: 1.0; color: "#FF111363" }
        }
    }

    Keys.onPressed: {
        print("Key pressed: ", event.key)
        if (event.key == Qt.Key_S) {
            print("Grab screen image")
            event.accepted = true

            root.grabToImage(function(result) {
                print("Grab screen image callback")
                print(FileManager.createFileName(FileManager.PICTURE))
                result.saveToFile(FileManager.createFileName(FileManager.PICTURE))
            });
        } else if (event.key == Qt.Key_W) {
            print("Grab waterfall image")
            event.accepted = true

            ping1DVisualizer.waterfallItem.grabToImage(function(result) {
                print("Grab waterfall image callback")
                print(FileManager.createFileName(FileManager.PICTURE))
                result.saveToFile(FileManager.createFileName(FileManager.PICTURE))
            });
        }
    }
}
