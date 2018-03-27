import QtGraphicalEffects 1.0
import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0
import Ping 1.0

Item {
    id: mainPage
    visible: true

    Column {
        z: 1
        anchors.left: parent.left
        anchors.top: parent.top

        PingItem {
            id: menuContainer
            icon: "/icons/arrow_right_white.svg"
            item:  GridLayout {
                columns: 5
                rowSpacing: 5
                columnSpacing: 5

                PingButton {
                    text: "Emit Ping"
                    //requestEchosounderProfile
                    onClicked: ping.request(1102)
                }

                Slider {
                    id: pingHzSlider
                    from: 0
                    value: 0
                    stepSize: 1
                    to: 35
                    Layout.columnSpan:  3
                    onValueChanged: {
                        pingPerSecond.text = Math.floor(value).toString() + " ping/s"
                        var period = 1000/value

                        ping.msec_per_ping = period

                        if(isNaN(period) || period <= 0) {
                            pingTimer.stop()
                            return
                        }
                        pingTimer.start()
                        pingTimer.interval = period
                    }
                }

                Text {
                    id: pingPerSecond
                    text: "0 ping/s"
                    color: Style.textColor
                }

                CheckBox {
                    id: autoGainChB
                    text: "Auto Gain"
                    checked: ping.modeAuto
                    onCheckedChanged: {
                        //setEchosounderAuto(checked)
                        ping.request(1102)
                    }
                }

                ComboBox {
                    id: gainCB
                    currentIndex: ping.gain
                    model: [0.5, 1.4, 4.3, 10, 23.4, 71, 166, 338, 794, 1737]
                    enabled: !autoGainChB.checked
                    Layout.columnSpan:  4
                    Layout.fillWidth: true
                    onCurrentIndexChanged: {
                        displayText = model[currentIndex] + " dB"
                    }
                    onActivated: {
                        //setEchosounderGain(index)
                    }
                }
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
            icon: "/icons/settings_white.svg"
            item: SettingsPage {
                id: settingsPage
                ping: ping
                waterfallItem: ping1DVisualizer.waterfallItem
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
            item: InfoPage {
                id: infoPage
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
        visible: settingsPage.replayItem
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
                    if(ping.link.packageIndex != value) {
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
            ping.connectLink("1:"+replayFileDialog.fileUrl.toString().slice(7)+":r")
            replayFileName.text = "File: " + replayFileDialog.fileUrl.toString().slice(sizeToRemove)
        }
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

        function firstRequest() {
            //requestEchosounderMode
            ping.request(1111)
            //requestEchosounderProfile
            ping.request(1102)
            //requestVersion
            ping.request(101)
            //requestDeviceID
            ping.request(120)
            //requestNewData // Does not exist
            //ping.request(112)
        }
    }

    Connections {
        target: ping
        onPointsUpdate: {
            ping1DVisualizer.draw(ping.points)
        }

        onDistanceUpdate: {
            ping1DVisualizer.setDepth(ping.distance/1e3)
        }

        onConfidenceUpdate: {
            // Q_PROPERTY does not exist
            //ping1DVisualizer.setConfidence(ping.confidence)
        }
    }

    Timer {
        id: pingTimer
        interval: 500; running: false; repeat: true
        // TODO get message ID enums in qml
        onTriggered: ping.request(1102)
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
    }

    LinearGradient {
        anchors.fill: parent
        z: parent.z-1
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#FF11b3ff" }
            GradientStop { position: 1.0; color: "#FF111363" }
        }
    }
}
