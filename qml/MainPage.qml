import AbstractLinkNamespace 1.0
import DeviceManager 1.0
import FileManager 1.0
import Ping 1.0
import PingEnumNamespace 1.0
import Qt.labs.settings 1.0
import Qt5Compat.GraphicalEffects
import QtMultimedia 5.12
import QtQuick 2.15
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs
import QtQuick.Layouts 1.3
import SettingsManager 1.0
import StyleManager 1.0

Item {
    id: root

    property var ping: DeviceManager.primarySensor
    property var sensorVisualizer: ping ? ping.sensorVisualizer(root) : null

    function playScreenshotAnimation() {
        cameraShutterSound.play();
        shutterAnimation.start();
    }

    visible: true
    focus: true
    Keys.onPressed: {
        print("Key pressed: ", event.key);
        if (event.key == Qt.Key_S) {
            print("Grab screen image");
            event.accepted = true;
            playScreenshotAnimation();
            root.grabToImage(function(result) {
                print("Grab screen image callback");
                result.saveToFile(FileManager.createFileName(FileManager.Pictures));
            });
            return ;
        }
        if (sensorVisualizer == null) {
            print("No visualizer to handle shortcut.");
            return ;
        }
        switch (event.key) {
        case Qt.Key_W:
            print("Grab visualizer image.");
            event.accepted = true;
            playScreenshotAnimation();
            sensorVisualizer.captureVisualizer();
            break;
        case Qt.Key_R:
            print("Clear visualizer.");
            event.accepted = true;
            sensorVisualizer.clear();
            break;
        default:
            print("Ask the visualizer to handle shortcut.");
            event.accepted = sensorVisualizer.handleShortcut(event.key);
        }
    }

    Column {
        z: 1
        anchors.left: parent.left
        anchors.top: parent.top

        NoControlPanel {
            id: noControlPanel

            visible: false
        }

        PingItem {
            id: menuContainer

            icon: StyleManager.arrowIcon()
            item: root.ping != null ? root.ping.controlPanel(menuContainer) : noControlPanel
            onHideItemChanged: {
                if (hideItem == false) {
                    settingsMenu.hideItem = true;
                    infoMenu.hideItem = true;
                } else {
                    if (settingsMenu.hideItem && infoMenu.hideItem)
                        root.focus = true;

                }
            }
        }

        PingItem {
            id: settingsMenu

            margin: 0
            icon: StyleManager.settingsIcon()
            onHideItemChanged: {
                if (hideItem == false) {
                    menuContainer.hideItem = true;
                    infoMenu.hideItem = true;
                } else {
                    if (menuContainer.hideItem && infoMenu.hideItem)
                        root.focus = true;

                }
            }

            item: ColumnLayout {
                spacing: 0

                PingItem {
                    id: deviceManagerMenu

                    isSubItem: true
                    icon: StyleManager.connectIcon()
                    onClickedChanged: {
                        if (clicked) {
                            deviceManagerViewer.open();
                            settingsMenu.hideItem = true;
                        }
                    }
                    onHideItemChanged: {
                        if (hideItem == false) {
                            displayItem.hideItem = true;
                            firmwareUpdateItem.hideItem = true;
                        }
                    }
                }

                PingItem {
                    id: displayItem

                    isSubItem: true
                    icon: StyleManager.sunIcon()
                    onHideItemChanged: {
                        if (hideItem == false) {
                            deviceManagerMenu.hideItem = true;
                            firmwareUpdateItem.hideItem = true;
                        }
                    }

                    item: DisplaySettings {
                        id: displaySettings
                    }

                }

                PingItem {
                    id: firmwareUpdateItem

                    isSubItem: true
                    icon: StyleManager.firmwareUpdateIcon()
                    onHideItemChanged: {
                        if (hideItem == false) {
                            deviceManagerMenu.hideItem = true;
                            displayItem.hideItem = true;
                        }
                    }

                    item: FirmwareUpdate {
                        id: firmwareUpdate

                        visible: false
                        ping: root.ping
                    }

                }

            }

        }

        PingItem {
            id: infoMenu

            icon: StyleManager.infoIcon()
            state: "fill-right"
            onHideItemChanged: {
                if (hideItem == false) {
                    menuContainer.hideItem = true;
                    settingsMenu.hideItem = true;
                } else {
                    if (menuContainer.hideItem && settingsMenu.hideItem)
                        root.focus = true;

                }
            }

            item: InfoPage {
                id: infoPage

                anchors.fill: parent
                deviceFirmware: ping ? ping.firmware_version_major + "." + ping.firmware_version_minor : null
                deviceID: ping ? ping.srcId : null
                deviceRevision: ping ? ping.device_revision : null
                deviceType: ping ? ping.device_type : null
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
        onHideItemChanged: {
            if (hideItem)
                root.focus = true;

        }

        item: RowLayout {
            PingButton {
                id: replayStartBt

                text: "▮▮"
                enabled: ping ? !ping.link.isWritable() : false
                onClicked: {
                    if (text == "▶") {
                        text = "▮▮";
                        ping.link.start();
                    } else {
                        text = "▶";
                        ping.link.pause();
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

                enabled: ping ? !ping.link.isWritable() : false
                from: 0
                value: ping ? ping.link.packageIndex : 0
                stepSize: 1
                to: ping ? ping.link.packageSize : 0
                onValueChanged: {
                    if (ping.link.packageIndex !== value) {
                        ping.link.packageIndex = value;
                        ping.link.start();
                        replayStartBt.text = "▮▮";
                    }
                    if (ping.link.packageIndex === 0)
                        replayStartBt.text = "▶";

                }
            }

            Text {
                id: replayElapsed

                text: ping ? ping.link.isWritable() ? "00:00:00.000 / 00:00:00.000" : ping.link.elapsedTimeString + " / " + ping.link.totalTimeString : null
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
            DeviceManager.playLogFile(AbstractLinkNamespace.File, [fileUrl, "r"]);
            replayFileName.text = "File: " + fileName;
        }
    }

    PingNotificationArea {
        id: pingNotificationArea

        z: 2
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    Loader {
        id: backGroundLoader

        objectName: "backGround"
        z: parent.z - 1
        anchors.fill: parent
        // Load linear or radial background
        // TODO: Add a proper check in PingSensor
        sourceComponent: sensorVisualizer == null ? radialGradient : sensorVisualizer.toString().includes("Ping360") ? radialGradient : linearGradient
    }

    PingStatus {
        // We need to be over the mainvisualizer
        z: sensorVisualizer ? sensorVisualizer.z + 1 : 0
        visible: SettingsManager.debugMode
    }

    // Linear and radial background gradients for different sensors
    Component {
        id: linearGradient

        LinearGradient {
            anchors.fill: parent

            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#FF11b3ff"
                }

                GradientStop {
                    position: 1
                    color: "#FF111363"
                }

            }

        }

    }

    Component {
        id: radialGradient

        RadialGradient {
            anchors.fill: parent

            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#FF11b3ff"
                }

                GradientStop {
                    position: 1
                    color: "#FF111363"
                }

            }

        }

    }

    // Sound effect for screenshot
    /*
    SoundEffect {
        id: cameraShutterSound

        source: "qrc:/sounds/camera-shutter.wav"
    }*/

    // Flash effect for screenshot
    Rectangle {
        anchors.fill: root
        opacity: 0
        color: "lightsteelblue"

        OpacityAnimator {
            id: shutterAnimation

            target: parent
            from: 0
            to: 1
            duration: 200
            loops: 1
        }

    }

}
