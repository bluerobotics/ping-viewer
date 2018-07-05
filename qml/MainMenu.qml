import QtGraphicalEffects 1.0
import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0
import Util 1.0

import Ping1DNamespace 1.0

Item {
    id: firmwareUpdate
    visible: false
    height: settingsLayout.height
    width: settingsLayout.width
    property var ping: null
    property var serialPortList: null

    function connect(first, second) {
        // Only connect from user input
        if(!firmwareUpdate.visible) {
            return;
        }

        // Do not connect if no valid type or input
        if(conntype.currentIndex < 0 && first !== "" && second !== "") {
            return;
        }

        // None = 0, File, Serial, Udp, Tcp, Sim
        // Enum Type : arg[0] : arg[1s]
        var connString = conntype.currentIndex == 2
                            ? ["5", "-", "-"]
                            : [(conntype.currentIndex + 2).toString(), first, second]

        ping.connectLink(connString)
    }


    ColumnLayout {
        id: settingsLayout
        GroupBox {
            id: pingGroup
            title: "Ping"
            enabled: ping.connected
            // Hack
            label.x: width/2 - label.contentWidth/2
            Layout.fillWidth: true

            ColumnLayout {
                spacing: 5
                width: parent.width

                RowLayout {
                    spacing: 5

                    PingButton {
                        text: "Emit Ping"
                        //requestEchosounderProfile
                        onClicked: ping.request(Ping1DNamespace.Profile)
                    }

                    Slider {
                        id: pingHzSlider
                        from: 0
                        stepSize: 1
                        to: 30
                        value: ping.pollFrequency;
                        onValueChanged: {
                            if (ping.pollFrequency !== value) {
                                ping.pollFrequency = value
                            }
                        }
                    }

                    Text {
                        id: pingPerSecond
                        text: Math.round(ping.pollFrequency) + " ping/s"
                        color: Style.textColor
                    }
                }

                RowLayout {
                    spacing: 5

                    CheckBox {
                        id: autoGainChB
                        text: "Auto Gain"
                        checked: ping.mode_auto
                        onCheckedChanged: {
                            ping.mode_auto = checked
                        }
                    }

                    ComboBox {
                        id: gainCB
                        currentIndex: ping.gain_index ? ping.gain_index : 0
                        model: [0.5, 1.4, 4.3, 10, 23.4, 71, 166, 338, 794, 1737]
                        enabled: !autoGainChB.checked
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                        onCurrentIndexChanged: {
                            displayText = model[currentIndex] + " dB"
                        }
                        onActivated: {
                            ping.gain_index = currentIndex
                        }
                    }
                }

                RowLayout {
                    spacing: 2
                    Text {
                        text: "Speed of Sound (m/s):"
                        color: Style.textColor
                    }

                    PingTextField {
                        id: speedOfSound
                        text: ""
                        validator: DoubleValidator {
                            // Values in m/s
                            bottom: SettingsManager.debugMode ? 0 : 1400
                            top: SettingsManager.debugMode ? 1e5 : 1600
                        }
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        onEditingFinished: {
                            var speed_of_sound = parseFloat(speedOfSound.text)
                            ping.speed_of_sound = speed_of_sound * 1000 // mm/s
                        }
                        Connections {
                            target: ping
                            onSpeedOfSoundUpdate: {
                                if (!speedOfSound.focus) {
                                    speedOfSound.text = ping.speed_of_sound / 1000
                                }
                            }
                        }
                    }
                }

                RowLayout {
                    spacing: 5
                    enabled: !autoGainChB.checked

                    Text {
                        text: "Start/Stop (mm):"
                        color: Style.textColor
                    }

                    PingTextField {
                        id: startLength
                        text: ""
                        validator: IntValidator{bottom: 0; top: 70000;}
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        onEditingFinished: {
                            var length_mm = parseInt(totalLength.text)
                            var start_mm = Math.min(parseInt(text), length_mm - 500)
                            if(isNaN(start_mm)) {
                                start_mm = 0
                            }
                            text = start_mm
                            ping.start_mm = start_mm
                        }
                        Connections {
                            target: ping
                            onLengthMmUpdate: {
                                if (!startLength.focus) {
                                    startLength.text = ping.start_mm
                                }
                            }
                        }
                    }

                    PingTextField {
                        id: totalLength
                        text: ""
                        validator: IntValidator{bottom: 0; top: 70000;}
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        onEditingFinished: {
                            var start_mm = parseInt(startLength.text)
                            var length_mm = Math.max(parseInt(text), start_mm + 500)
                            if(isNaN(length_mm)) {
                                length_mm = 48903
                            }
                            text = length_mm
                            ping.length_mm = length_mm
                        }
                        Connections {
                            target: ping
                            onLengthMmUpdate: {
                                if (!totalLength.focus) {
                                    totalLength.text = ping.length_mm
                                }
                            }
                        }
                    }
                }
            }
        }

        GroupBox {
            id: connGroup
            title: "Connection"
            enabled: true
            // Hack
            label.x: width/2 - label.contentWidth/2
            Layout.fillWidth: true

            GridLayout {
                anchors.fill: parent
                columns: 5
                rowSpacing: 5
                columnSpacing: 5

                Text {
                    text: "Sonar Type:"
                    color: Style.textColor
                }

                ComboBox {
                    displayText: "Ping Echosounder"
                    enabled: false
                    Layout.columnSpan:  4
                    Layout.fillWidth: true
                }

                Text {
                    text: "Communication:"
                    enabled: true
                    color: Style.textColor
                }

                ComboBox {
                    id: conntype
                    enabled: true
                    Layout.columnSpan:  4
                    Layout.fillWidth: true
                    model: ["Serial (default)", "UDP", "Simulation"]
                    onActivated: {
                        switch(index) {
                            case 0: // Serial
                                udpLayout.enabled = false
                                serialLayout.enabled = true
                                connect(serialPortsCB.currentText, baudrateBox.currentText)
                                break

                            case 1: // UDP
                                udpLayout.enabled = true
                                serialLayout.enabled = false
                                connect(udpIp.text, udpPort.text)
                                break

                            case 2:
                                udpLayout.enabled = false
                                serialLayout.enabled = false
                                connect()
                        }
                    }
                }

                RowLayout {
                    id: serialLayout
                    spacing: 5
                    Layout.columnSpan:  5

                    Text {
                        id: font
                        text: "Serial Port / Baud:"
                        color: Style.textColor
                    }

                    ComboBox {
                        id: serialPortsCB
                        Layout.columnSpan:  2
                        Layout.fillWidth: true
                        model: serialPortList
                        onActivated: {
                            if (currentIndex > -1) {
                                connect(serialPortsCB.currentText, baudrateBox.currentText)
                            }
                        }

                        onModelChanged: {
                            var maxWidth = width
                            for(var i in model) {
                                var modelWidth = (model[i].length+1)*font.font.pixelSize
                                maxWidth = maxWidth < modelWidth ? modelWidth : maxWidth
                            }
                            popup.width = maxWidth
                        }
                        onPressedChanged: {
                            if(pressed) {
                                serialPortList = Util.serialPortList()
                            }
                        }
                        Component.onCompleted: serialPortList = Util.serialPortList()
                    }

                    ComboBox {
                        id: baudrateBox
                        model: [115200, 921600]
                        onActivated: {
                            connect(serialPortsCB.currentText, baudrateBox.currentText)
                        }
                    }
                }

                RowLayout {
                    id: udpLayout
                    spacing: 5
                    Layout.columnSpan:  5

                    Text {
                        text: "UDP Host/Port:"
                        color: Style.textColor
                    }

                    PingTextField {
                        id: udpIp
                        text: "192.168.2.2"
                        enabled: false
                        Layout.columnSpan:  2
                        Layout.fillWidth: true
                        onTextChanged: {
                            connect(udpIp.text, udpPort.text)
                        }
                    }

                    PingTextField {
                        id: udpPort
                        text: "1234"
                        enabled: false
                        Layout.columnSpan:  2
                        Layout.fillWidth: true
                        onTextChanged: {
                            connect(udpIp.text, udpPort.text)
                        }
                    }
                }
            }
        }
    }
}
