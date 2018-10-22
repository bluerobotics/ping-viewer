import QtGraphicalEffects 1.0
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

import AbstractLinkNamespace 1.0
import FileManager 1.0
import Ping 1.0
import Ping1DNamespace 1.0
import SettingsManager 1.0
import StyleManager 1.0

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
            icon: StyleManager.arrowIcon()
            item: MainMenu {
                ping: ping
            }

            onHideItemChanged: {
                if(hideItem == false) {
                    settingsMenu.hideItem = true
                    infoMenu.hideItem = true
                } else {
                    if(settingsMenu.hideItem && infoMenu.hideItem) {
                        root.focus = true
                    }
                }
            }
        }

        PingItem {
            id: settingsMenu
            marginMult: 1
            icon: StyleManager.settingsIcon()
            item: ColumnLayout {
                spacing: 0
                PingItem {
                    id: displayItem
                    isSubItem: true
                    icon: StyleManager.sunIcon()

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
                    icon: StyleManager.chipIcon()

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
                } else {
                    if(menuContainer.hideItem && infoMenu.hideItem) {
                        root.focus = true
                    }
                }
            }
        }

        PingItem {
            id: infoMenu
            icon: StyleManager.infoIcon()
            state: "fill-right"
            item: InfoPage {
                id: infoPage
                anchors.fill: parent
                deviceFirmware: ping.firmware_version_major + "." + ping.firmware_version_minor
                deviceID: ping.srcId
                deviceModel: ping.device_model
                deviceType: ping.device_type
            }
            onHideItemChanged: {
                if(hideItem == false) {
                    menuContainer.hideItem = true
                    settingsMenu.hideItem = true
                } else {
                    if(menuContainer.hideItem && settingsMenu.hideItem) {
                        root.focus = true
                    }
                }
            }
        }
    }

    PingItem {
        id: replayMenu
        visible: SettingsManager.replayMenu
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        state: "bottom-left"
        spin: true
        icon: StyleManager.diskIcon()
        item: RowLayout {
            PingButton {
                id: replayStartBt
                text: "▮▮"
                enabled: !ping.link.isWritable()
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
                color: Material.primary
            }

            Slider {
                id: replaySlider
                enabled: !ping.link.isWritable()
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
                text: ping.link.isWritable() ? "00:00:00.000 / 00:00:00.000" : ping.link.elapsedTimeString + " / " + ping.link.totalTimeString
                color: Material.primary
            }

            Text {
                id: replayFileName
                text: "File:"
                color: Material.primary
            }

            PingButton {
                text: "Replay Data"
                onClicked: replayFileDialog.visible = true
            }
        }

        onHideItemChanged: {
            if(hideItem) {
                root.focus = true
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
            ping.connectLink(AbstractLinkNamespace.File, [finalString, "r"])
            replayFileName.text = "File: " + replayFileDialog.fileUrl.toString().slice(sizeToRemove)
        }
    }

    PingNotificationArea {
        id: pingNotificationArea
        z: 2
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    Ping {
        id: ping

        onPointsUpdate: {
            // Move from mm to m
            ping1DVisualizer.draw(ping.points, ping.confidence, ping.start_mm*1e-3, ping.length_mm * 1e-3, ping.distance*1e-3)
        }

        onDistanceUpdate: {
            ping1DVisualizer.setDepth(ping.distance/1e3)
        }

        onConfidenceUpdate: {
            ping1DVisualizer.setConfidence(ping.confidence)
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
        visible: SettingsManager.debugMode
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
                print(FileManager.createFileName(FileManager.Pictures))
                result.saveToFile(FileManager.createFileName(FileManager.Pictures))
            });
        } else if (event.key == Qt.Key_W) {
            print("Grab waterfall image")
            event.accepted = true

            ping1DVisualizer.waterfallItem.grabToImage(function(result) {
                print("Grab waterfall image callback")
                print(FileManager.createFileName(FileManager.Pictures))
                result.saveToFile(FileManager.createFileName(FileManager.Pictures))
            });
        } else if (event.key == Qt.Key_R) {
            print("Clear waterfall")
            ping1DVisualizer.waterfallItem.clear()
        }
    }
}
