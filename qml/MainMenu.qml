import QtGraphicalEffects 1.0
import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0
import Util 1.0

Item {
    id: firmwareUpdate
    visible: false
    height: settingsLayout.height
    width: settingsLayout.width
    property var ping: null
    property var serialPortList: null


    Timer {
        interval: 1000
        repeat: true
        onTriggered: {
            serialPortList = Util.serialPortList()
        }

        Component.onCompleted: start()
    }

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
                            ? "5:-:-"
                            : (conntype.currentIndex + 2).toString() + ":" + first + ":" + second

        ping.connectLink(connString)
    }


    ColumnLayout {
        id: settingsLayout
        GroupBox {
            id: pingGroup
            title: "Ping"
            enabled: true
            // Hack
            label.x: width/2 - label.contentWidth/2
            Layout.fillWidth: true

            GridLayout {
                columns: 5
                rowSpacing: 5
                columnSpacing: 5

                PingButton {
                    text: "Emit Ping"
                    //requestEchosounderProfile
                    onClicked: ping.request(1300)
                }

                Slider {
                    id: pingHzSlider
                    from: 0
                    stepSize: 1
                    to: 30
                    Layout.columnSpan:  3
                    value: ping.pollFrequency;
                    onValueChanged: {
                        if (ping.pollFrequency !== value) {
                            ping.pollFrequency = value
                        }
                    }
                }

                Text {
                    id: pingPerSecond
                    text: Math.round(ping.pollFrequency) + "ping/s"
                    color: Style.textColor
                }

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
                    currentIndex: ping.gain_index
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
                Text {
                    text: "Start/Stop (mm):"
                    color: Style.textColor
                }

                PingTextField {
                    id: startLength
                    text: ""
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    onEditingFinished: {
                        if (parseInt(text)) {
                            ping.start_mm = parseInt(text)
                        }
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
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    onEditingFinished: {
                        if (parseInt(text)) {
                            ping.length_mm = parseInt(text)
                        }
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
                                udpIp.enabled = false
                                udpPort.enabled = false
                                serialPortsCB.enabled = true
                                baudrateBox.enabled = true
                                connect(serialPortsCB.currentText, baudrateBox.currentText)
                                break

                            case 1: // UDP
                                udpIp.enabled = true
                                udpPort.enabled = true
                                serialPortsCB.enabled = false
                                baudrateBox.enabled = false
                                connect(udpIp.text, udpPort.text)
                                break

                            case 2:
                                udpIp.enabled = false
                                udpPort.enabled = false
                                serialPortsCB.enabled = false
                                baudrateBox.enabled = false
                                connect()
                        }
                    }
                }

                Text {
                    text: "Serial Port / Baud:"
                    color: Style.textColor
                }

                ComboBox {
                    id: serialPortsCB
                    Layout.columnSpan:  3
                    Layout.fillWidth: true
                    model: serialPortList
                    onActivated: {
                        if (currentIndex > -1) {
                            connect(serialPortsCB.currentText, baudrateBox.currentText)
                        }
                    }
                }

                ComboBox {
                    id: baudrateBox
                    model: [115200, 921600]
                    onActivated: {
                        connect(serialPortsCB.currentText, baudrateBox.currentText)
                    }
                }

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
