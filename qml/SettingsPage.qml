import QtGraphicalEffects 1.0
import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

Item {
    id: settingsPage
    visible: false
    height: settingsLayout.height
    width: settingsLayout.width

    property var ping
    function connect(first, second) {
        // Do not connect if no type selected
        if(conntype.currentIndex < 0) {
            return;
        }

        // None = 0, File, Serial, Udp, Tcp
        // Enum Type : arg[0] : arg[1s]
        var connString = (conntype.currentIndex + 2).toString() + ":" + first + ":" + second
        ping.connectLink(connString)
    }

    ColumnLayout {
        id: settingsLayout
        RowLayout {
            GroupBox {
                id: syncGroup
                title: "Synchronize"
                // Don't work
                // label.anchors.horizontalCenter: horizontalCenter
                // Hack
                label.x: width/2 - label.contentWidth/2
                GridLayout {
                    anchors.fill: parent
                    columns: 5
                    rowSpacing: 5
                    columnSpacing: 5

                    Text {
                        text: "Sonar Type:"
                        color: 'linen'
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
                        color: 'linen'
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
                        color: 'linen'
                    }

                    ComboBox {
                        id: serialPortsCB
                        Layout.columnSpan:  2
                        Layout.fillWidth: true
                        model: ping.link.listAvailableConnections
                    }

                    ComboBox {
                        id: baudrateBox
                        model: [115200, 921600]
                    }

                     Button {
                        text: "Ok"
                        enabled: true
                        Layout.fillWidth: true
                        onClicked: {
                            connect(serialPortsCB.currentText, baudrateBox.currentText)
                        }
                    }

                    Text {
                        text: "UDP Host/Port:"
                        color: 'linen'
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
                        color: 'linen'
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

            GroupBox {
                id: displayGroup
                title: "Display"
                // Hack
                label.x: width/2 - label.contentWidth/2
                Layout.fillWidth: true

                GridLayout {
                    anchors.fill: parent
                    columns: 5
                    rowSpacing: 5
                    columnSpacing: 5

                    Text {
                        text: "Units:"
                        color: 'linen'
                    }

                    ComboBox {
                        displayText: "Meters"
                        enabled: false
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                        onCurrentTextChanged: {
                            PingSettings.unitType = currentText
                        }
                    }

                    Text {
                        text: "Plot Theme:"
                        color: 'linen'
                    }

                    ComboBox {
                        id: plotThemeCB
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                        Layout.minimumWidth: 200
                        model: PingSettings.plotThemesAvailable
                        onCurrentTextChanged: {
                            PingSettings.plotTheme = currentText
                        }
                        onModelChanged: {
                            currentIndex = settings.plotThemeIndex
                        }
                    }

                    Text {
                        text: "Theme:"
                        color: 'linen'
                    }

                    ComboBox {
                        displayText: "Dark"
                        enabled: false
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                        onCurrentTextChanged: {
                            PingSettings.theme = currentText
                        }
                    }

                    CheckBox {
                        id: replayChB
                        text: "Enable replay menu"
                        checked: false
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                        onCheckStateChanged: {
                            PingSettings.replayIsEnable = checkState
                        }
                        Component.onCompleted: {
                            PingSettings.replayIsEnable = checkState
                        }
                    }

                    CheckBox {
                        id: smoothDataChB
                        text: "Smooth Data"
                        checked: true
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                        onCheckStateChanged: {
                            PingSettings.smoothIsEnable = checkState
                        }
                        Component.onCompleted: {
                            PingSettings.smoothIsEnable = checkState
                        }
                    }
                }
            }
        }

        GroupBox {
            id: firmwareGroup
            title: "Firmware Update"
            enabled: false
            // Hack
            label.x: width/2 - label.contentWidth/2
            Layout.fillWidth: true

            GridLayout {
                anchors.fill: parent
                columns: 5
                rowSpacing: 5
                columnSpacing: 5

                Text {
                    text: "Current Firmware:"
                    color: 'linen'
                }

                TextField {
                    Layout.columnSpan:  4
                    Layout.fillWidth: true
                }

                Text {
                    id: firmwareLabel
                    text: "Firmware File:"
                    color: 'linen'
                }

                TextField {
                    Layout.columnSpan:  3
                    Layout.fillWidth: true
                }

                PingButton {
                    text: "Browse.."
                }

                PingButton {
                    text: "Firmware Update"
                    Layout.columnSpan:  5
                    Layout.fillWidth: true
                }

                Text {
                    text: "Progress:"
                    color: 'linen'
                }

                ProgressBar {
                    indeterminate: true
                    Layout.columnSpan:  4
                    Layout.fillWidth: true
                }
            }
        }
    }

    Settings {
        id: settings
        property var plotThemeIndex
        property alias replayItemChecked: replayChB.checkState
        property alias smoothDataState: smoothDataChB.checkState
    }


    Component.onCompleted: {
        plotThemeCB.currentIndex = settings.plotThemeIndex
    }

    Component.onDestruction: {
        settings.plotThemeIndex = plotThemeCB.currentIndex
    }
}