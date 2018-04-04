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
        running: firmwareUpdate.visible && conntype.currentIndex == 0
        repeat: true
        onTriggered: {
            serialPortList = Util.serialPortList()
        }
    }

    function connect(first, second) {
        // Only connect from user input
        if(!firmwareUpdate.visible) {
            return;
        }

        // Do not connect if no valid type or input
        if(conntype.currentIndex < 0 && first != "" && second != "") {
            return;
        }

        // None = 0, File, Serial, Udp, Tcp
        // Enum Type : arg[0] : arg[1s]
        var connString = (conntype.currentIndex + 2).toString() + ":" + first + ":" + second
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
                    model: ["Serial (default)", "UDP"]
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

                TextField {
                    id: udpIp
                    text: "192.168.2.2"
                    enabled: false
                    Layout.columnSpan:  2
                    Layout.fillWidth: true
                    onTextChanged: {
                        connect(udpIp.text, udpPort.text)
                    }
                }

                TextField {
                    id: udpPort
                    text: "1234"
                    enabled: false
                    Layout.columnSpan:  2
                    Layout.fillWidth: true
                    onTextChanged: {
                        connect(udpIp.text, udpPort.text)
                    }
                }

                Text {
                    text: "Save Path:"
                    color: Style.textColor
                }

                TextField {
                    id: pathField
                    enabled: false
                    Layout.columnSpan:  3
                    Layout.fillWidth: true
                }

                Button {
                    text: "Browse..."
                    enabled: false
                    // hack
                    implicitWidth: baudrateBox.width
                }
            }
        }
    }
}
