import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3

Item {
    id: root
    height: 50

    states: [
        State {
            name: "compact"
            PropertyChanges {
                target: configGroup
                visible: false
            }
        },
        State {
            name: "expand"
            PropertyChanges {
                target: configGroup
                visible: true
            }
        }
    ]

    Rectangle {
        color: "Black"
        opacity: 0.45
        border.width: 1
        anchors.fill: parent
    }

    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Layout.margins: 3
            Layout.fillWidth: true

            Text {
                text: "link@params"
                color: 'linen'
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                Layout.leftMargin: 10
            }

            Rectangle {
                height: 0
                Layout.fillWidth: true
            }

            Rectangle {
                height: 15
                width: height
                color: "red"
                Layout.alignment: Text.AlignVCenter
                border.color: "black"
                border.width: 1
                radius: height
            }

            Image {
                id: openIcon
                Layout.maximumHeight: 35
                source: "/icons/sub_settings_white.svg"
                fillMode: Image.PreserveAspectFit
                mipmap: true
                Layout.alignment: Qt.AlignVCenter
                visible: parent.visible

                MouseArea {
                    id: addLinkBt
                    anchors.fill: parent
                    onClicked: {
                        root.state = root.state == "expand" ? "compact" : "expand"
                    }
                }
            }
        }

        GroupBox {
            id: configGroup
            title: "Configuration"
            // Don't work
            // label.anchors.horizontalCenter: horizontalCenter
            // Hack
            label.x: width/2 - label.contentWidth/2
            Layout.fillWidth: true
            visible: false
            GridLayout {
                anchors.fill: parent
                columns: 5
                rowSpacing: 2
                columnSpacing: 2

                Text {
                    text: "Type:"
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
                                serialText.visible = true
                                udpText.visible = !serialText.visible
                                //connect(serialPortsCB.currentText, baudrateBox.currentText)
                                break

                            case 1: // UDP
                                serialText.visible = false
                                udpText.visible = !serialText.visible
                                //connect(udpIp.text, udpPort.text)
                                break
                        }
                    }
                }

                Text {
                    id: serialText
                    text: "Serial Port:"
                    color: 'linen'
                    visible: true
                }

                ComboBox {
                    id: serialPortsCB
                    Layout.columnSpan:  4
                    Layout.fillWidth: true
                    visible: serialText.visible
                    //model: ping.link.listAvailableConnections
                }

                Text {
                    id: baudrateText
                    text: "Baudrate:"
                    color: 'linen'
                    visible: serialText.visible
                }

                ComboBox {
                    id: baudrateBox
                    Layout.columnSpan:  4
                    Layout.fillWidth: true
                    visible: serialText.visible
                    model: [115200, 921600]
                }

                Text {
                    id: udpText
                    text: "UDP Host:"
                    color: 'linen'
                    visible: false
                }

                TextField {
                    id: udpIp
                    text: "192.168.2.2"
                    Layout.columnSpan:  4
                    Layout.fillWidth: true
                    visible: udpText.visible
                    onTextChanged: {
                        //connect(udpIp.text, udpPort.text)
                    }
                }

                Text {
                    id: portText
                    text: "Port:"
                    color: 'linen'
                    visible: udpText.visible
                }

                TextField {
                    id: udpPort
                    text: "1234"
                    Layout.columnSpan:  4
                    Layout.fillWidth: true
                    visible: udpText.visible
                    onTextChanged: {
                        //connect(udpIp.text, udpPort.text)
                    }
                }

                Button {
                    Layout.columnSpan:  1
                    onClicked: {
                        //connect(serialPortsCB.currentText, baudrateBox.currentText)
                    }

                    Image {
                        source: "/icons/delete_white.svg"
                        mipmap: true
                        fillMode: Image.PreserveAspectFit
                        anchors.fill: parent
                        Layout.alignment: Qt.AlignVCenter
                        visible: parent.visible
                    }
                }

                Rectangle {
                    height: 0
                    Layout.fillWidth: true
                    Layout.columnSpan:  3
                }

                Button {
                    Layout.columnSpan:  1
                    onClicked: {
                        //connect(serialPortsCB.currentText, baudrateBox.currentText)
                    }

                    Image {
                        source: "/icons/done_white.svg"
                        mipmap: true
                        fillMode: Image.PreserveAspectFit
                        anchors.fill: parent
                        Layout.alignment: Qt.AlignVCenter
                        visible: parent.visible
                    }
                }
            }

            onVisibleChanged: {
                if(visible) {
                    root.height += configGroup.height
                } else {
                    root.height -= configGroup.height
                }
            }
        }
    }
}