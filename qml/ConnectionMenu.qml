import QtQuick 2.11
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

import AbstractLinkNamespace 1.0
import SettingsManager 1.0
import Util 1.0

Item {
    id: root
    height: connectionLayout.height
    width: connectionLayout.width
    property var ping: null
    property var serialPortList: null

    function connect(connectionTypeEnum) {
        // Only connect from user input
        if(!parent.visible) {
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

    GroupBox {
        id: connectionLayout
        title: "Connection configuration"
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
                text: "Communication:"
                enabled: true
                color: Material.primary
            }

            ComboBox {
                id: conntype
                enabled: true
                Layout.columnSpan:  4
                Layout.fillWidth: true
                Layout.minimumWidth: 150
                // Check AbstractLinkNamespace::LinkType for correct index type
                // None = 0, File, Serial, Udp, Tcp, Sim...
                model: SettingsManager.debugMode ?
                    ["Serial (default)", "UDP", "Simulation"] : ["Serial (default)", "UDP"]
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
                    model: [9600, 115200]
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
