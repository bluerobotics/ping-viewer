import QtGraphicalEffects 1.0
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

import AbstractLinkNamespace 1.0
import DeviceManager 1.0
import FileManager 1.0
import Ping 1.0
import Ping1DNamespace 1.0
import SettingsManager 1.0
import StyleManager 1.0

Item {
    id: root
    visible: true
    focus: true

    property var ping: DeviceManager.primarySensor

    Column {
        z: 1
        anchors.left: parent.left
        anchors.top: parent.top

        NoControlPanel {
            id: noControlPanel
        }

        PingItem {
            id: menuContainer
            icon: StyleManager.arrowIcon()
            item: root.ping != null ? root.ping.controlPanel(menuContainer) : noControlPanel

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
                    }

                    onHideItemChanged: {
                        if(hideItem == false) {
                            deviceManagerMenu.hideItem = true
                        }
                    }
                }

                PingItem {
                    id: deviceManagerMenu
                    isSubItem: true
                    icon: StyleManager.connectIcon()

                    onClickedChanged: {
                        if(clicked) {
                            deviceManagerViewer.open()
                            settingsMenu.hideItem = true
                        }
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
                deviceRevision: ping.device_revision
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

    PingFileDialog {
        id: replayFileDialog
        title: "Please choose a log file"
        nameFilters: ["Binary files (*.bin)"]
        // Accessing folder variable directly does not work
        // From: https://stackoverflow.com/questions/46672657/how-to-set-filedialogs-folder-from-a-unc-path-in-qml
        // Bug: https://bugreports.qt.io/browse/QTBUG-63710
        Component.onCompleted: folder = FileManager.getPathFrom(FileManager.SensorLog)
        onAccepted: {
            ping.connectLink(AbstractLinkNamespace.File, [fileUrl, "r"])
            replayFileName.text = "File: " + fileName
        }
    }

    PingNotificationArea {
        id: pingNotificationArea
        z: 2
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    Loader {
        id: sensorVisualizerLoader
        objectName: "sensorVisualizer"
        anchors.fill: parent
        sourceComponent: DeviceManager.primarySensor.sensorVisualizer(parent)
    }

    PingStatus {
        ping: root.ping
        visible: SettingsManager.debugMode
    }

    Loader {
        id: backGroundLoader
        objectName: "backGround"
        z: parent.z-1
        anchors.fill: parent
        // Load linear or radial background
        // TODO: Add a proper check in PingSensor
        sourceComponent: sensorVisualizerLoader.item == null ?
            radialGradient : sensorVisualizerLoader.item.toString().includes("Ping360") ? radialGradient : linearGradient
    }

    // Linear and radial background gradients for different sensors
    Component {
        id: linearGradient
        LinearGradient {
            anchors.fill: parent
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#FF11b3ff" }
                GradientStop { position: 1.0; color: "#FF111363" }
            }
        }
    }
    Component {
        id: radialGradient
        RadialGradient {
            anchors.fill: parent
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#FF11b3ff" }
                GradientStop { position: 1.0; color: "#FF111363" }
            }
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
            })

        // This are the base shortcuts that are common between all visualization items
        } else if(event.key == Qt.Key_W) {
            print("Grab visualizer image.")
            event.accepted = true
            sensorVisualizerLoader.item.captureVisualizer()

        }  else if(event.key == Qt.Key_R) {
            print("Clear visualizer.")
            event.accepted = true
            sensorVisualizerLoader.item.clear()

        } else {
            // Let the visualizer use specialized shortcuts if necessary
            event.accepted = sensorVisualizerLoader.item.handleShortcut(event.key)
        }
    }
}
