import QtGraphicalEffects 1.0
import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0
import Util 1.0

import AbstractLinkNamespace 1.0
import Ping1DNamespace 1.0
import SettingsManager 1.0

Item {
    id: firmwareUpdate
    visible: false
    height: settingsLayout.height
    width: settingsLayout.width
    property var ping: null
    property var serialPortList: null

    function connect(connectionTypeEnum) {
        // Only connect from user input
        if(!firmwareUpdate.visible) {
            return;
        }

        // Do not connect if no valid type or input
        if(connectionTypeEnum < AbstractLinkNamespace.None || connectionTypeEnum >= AbstractLinkNamespace.Last) {
            print("The connection configuration type is not valid!")
            return;
        }

        // Transform arguments to Array and remove connectionTypeEnum from it
        var nextArgs = Array.prototype.slice.call(arguments).slice(1)
        ping.connectLink(connectionTypeEnum, nextArgs)
    }

    Connections {
        target: ping
        onLinkUpdate: {
            // Connection update
            // Update interface to make sure
            switch(ping.link.configuration.type()) {
                case AbstractLinkNamespace.Serial:
                    conntype.currentIndex = 0
                    udpLayout.enabled = false
                    serialLayout.enabled = true
                    serialPortsCB.currentText = ping.link.configuration.argsAsConst()[0]
                    baudrateBox.currentText = ping.link.configuration.argsAsConst()[1]
                    break;
                case AbstractLinkNamespace.Udp:
                    conntype.currentIndex = 1
                    udpLayout.enabled = true
                    serialLayout.enabled = false
                    udpIp.text = ping.link.configuration.argsAsConst()[0]
                    udpPort.text = ping.link.configuration.argsAsConst()[1]
                    break;
                case AbstractLinkNamespace.PinkSimulation:
                    conntype.currentIndex = 2
                    udpLayout.enabled = false
                    serialLayout.enabled = false
                    break;
                default:
                    print('Not valid link.')
                    print(ping.link.configuration.name())
                    print(ping.link.configuration.type())
                    print(ping.link.configuration.argsAsConst())
                    return;
            }
        }
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
                        to: ping.pingMaxFrequency
                        value: ping.pingFrequency;
                        onValueChanged: {
                            if (ping.pingFrequency !== value) {
                                ping.pingFrequency = value
                            }
                        }
                    }

                    Text {
                        id: pingPerSecond
                        text: Math.round(ping.pingFrequency) + " ping/s"
                        color: Material.primary
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
                        model: [-4.4, 5.2, 14.8, 22.2, 29.6, 36.4, 43.2]
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
                        color: Material.primary
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
                        text: "Start/Length (mm):"
                        color: Material.primary
                    }

                    PingTextField {
                        id: startLength
                        text: ""
                        validator: IntValidator {
                            bottom: 0
                            top: SettingsManager.debugMode ? 1e6 : 7e5;
                        }
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        onEditingFinished: {
                            var start_mm = parseInt(text)
                            text = start_mm
                            ping.start_mm = start_mm
                        }
                        Connections {
                            target: ping
                            onScanStartUpdate: {
                                if (!startLength.focus) {
                                    startLength.text = ping.start_mm
                                }
                            }
                        }
                    }

                    PingTextField {
                        id: totalLength
                        text: ""
                        validator: IntValidator {
                            bottom: 0
                            top: SettingsManager.debugMode ? 1e6 : 7e5;
                        }
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        onEditingFinished: {
                            var start_mm = parseInt(startLength.text)
                            var length_mm = Math.min(parseInt(text), 50000 - start_mm)
                            if(isNaN(length_mm)) {
                                length_mm = 500
                            }
                            text = length_mm
                            ping.length_mm = length_mm
                        }
                        Connections {
                            target: ping
                            onScanLengthUpdate: {
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
                    color: Material.primary
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
                    color: Material.primary
                }

                ComboBox {
                    id: conntype
                    enabled: true
                    Layout.columnSpan:  4
                    Layout.fillWidth: true
                    // Check AbstractLinkNamespace::LinkType for correct index type
                    // None = 0, File, Serial, Udp, Tcp, Sim...
                    model: ["Serial (default)", "UDP", "Simulation"]
                    onActivated: {
                        switch(index) {
                            case 0: // Serial
                                udpLayout.enabled = false
                                serialLayout.enabled = true
                                connect(AbstractLinkNamespace.Serial, serialPortsCB.currentText, baudrateBox.currentText)
                                break

                            case 1: // UDP
                                udpLayout.enabled = true
                                serialLayout.enabled = false
                                connect(AbstractLinkNamespace.Udp, udpIp.text, udpPort.text)
                                break

                            case 2: // Simulation
                                udpLayout.enabled = false
                                serialLayout.enabled = false
                                connect(AbstractLinkNamespace.PingSimulation)
                        }
                    }
                }

                RowLayout {
                    id: serialLayout
                    visible: serialLayout.enabled
                    spacing: 5
                    Layout.columnSpan:  5

                    Text {
                        id: font
                        text: "Serial Port / Baud:"
                        color: Material.primary
                    }

                    ComboBox {
                        id: serialPortsCB
                        Layout.columnSpan:  2
                        Layout.fillWidth: true
                        model: serialPortList
                        onActivated: {
                            if (currentIndex > -1) {
                                connect(AbstractLinkNamespace.Serial, serialPortsCB.currentText, baudrateBox.currentText)
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
                            connect(AbstractLinkNamespace.Serial, serialPortsCB.currentText, baudrateBox.currentText)
                        }
                    }
                }

                RowLayout {
                    id: udpLayout
                    visible: udpLayout.enabled
                    spacing: 5
                    Layout.columnSpan:  5
                    enabled: false

                    Text {
                        text: "UDP Host/Port:"
                        color: Material.primary
                    }

                    PingTextField {
                        id: udpIp
                        text: "192.168.2.2"
                        Layout.columnSpan:  2
                        Layout.fillWidth: true
                        onEditingFinished: {
                            if (udpIp.text == "0.0.0.0" || udpIp.text == "localhost") {
                                udpIp.text = "127.0.0.1"
                            }
                            connect(AbstractLinkNamespace.Udp, udpIp.text, udpPort.text)
                        }
                    }

                    PingTextField {
                        id: udpPort
                        text: "1234"
                        Layout.columnSpan:  2
                        Layout.fillWidth: true
                        onEditingFinished: {
                            connect(AbstractLinkNamespace.Udp, udpIp.text, udpPort.text)
                        }
                    }
                }
            }
        }
    }
}
