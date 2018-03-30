import QtGraphicalEffects 1.0
import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0
import Util 1.0

Item {
    id: settingsPage
    visible: false
    height: settingsLayout.height
    width: settingsLayout.width

    property var ping
    property var waterfallItem
    property var replayItem: replayChB.checked
    property var serialPortList

    Timer {
        interval: 1000
        running: settingsPage.visible && conntype.currentIndex == 0
        repeat: true
        onTriggered: {
            serialPortList = Util.serialPortList()
        }
    }

    Connections {
        target: ping
        onFlashProgress: {
            flashProgress.indeterminate = false;
            flashProgress.value = progress;
        }
        onFlashComplete: {
            console.log("flash complete!")
            flashProgress.value = 0.0;
            flashProgress.indeterminate = true;
        }
    }

    function connect(first, second) {
        // Only connect from user input
        if(!settingsPage.visible) {
            return;
        }

        // Do not connect if no valid type or input
        if(conntype.currentIndex < 0 && first != "" && second != "") {
            return;
        }

        // None = 0, File, Serial, Udp, Tcp, Sim
        // Enum Type : arg[0] : arg[1s]
        var connString = conntype.currentIndex == 2
                                ? "5:-:-"
                                : (conntype.currentIndex + 2).toString() + ":" + first + ":" + second

        console.log(connString)
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
                        color: Style.textColor
                    }

                    ComboBox {
                        displayText: "Meters"
                        enabled: false
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                    }

                    Text {
                        text: "Plot Theme:"
                        color: Style.textColor
                    }

                    ComboBox {
                        id: plotThemeCB
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                        Layout.minimumWidth: 200
                        model: waterfallItem.themes
                        onCurrentTextChanged: {
                            waterfallItem.theme = currentText
                        }
                    }

                    Text {
                        text: "Theme:"
                        color: Style.textColor
                    }

                    ComboBox {
                        id: themeCB
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                        model: ["Dark", "Light"]
                        onCurrentTextChanged: {
                            if (currentIndex) {
                                Style.useLightStyle()
                            } else {
                                Style.useDarkStyle()
                            }
                        }
                    }

                    CheckBox {
                        id: replayChB
                        text: "Enable replay menu"
                        checked: false
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                    }

                    CheckBox {
                        id: smoothDataChB
                        text: "Smooth Data"
                        checked: true
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                        onCheckStateChanged: {
                            waterfallItem.smooth = checkState
                        }
                    }
                }
            }
        }

        GroupBox {
            id: firmwareGroup
            title: "Firmware Update"
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
                    text: "Current Firmware:"
                    color: Style.textColor
                }

                TextField {
                    enabled: false
                    Layout.columnSpan:  4
                    Layout.fillWidth: true
                }

                Text {
                    id: firmwareLabel
                    text: "Firmware File:"
                    color: Style.textColor
                }

                TextField {
                    id: firmwareFileName
                    enabled: false
                    Layout.columnSpan:  3
                    Layout.fillWidth: true
                    text: fileDialog.fileName
                }

                PingButton {
                    text: "Browse.."
                    enabled: flashProgress.indeterminate
                    onClicked: {
                        fileDialog.visible = true
                    }
                }

                PingButton {
                    text: "Firmware Update"
                    Layout.columnSpan:  5
                    Layout.fillWidth: true
                    enabled: fileDialog.fileUrl.toString().length && flashProgress.indeterminate

                    onClicked: {
                        ping.firmwareUpdate(fileDialog.fileUrl)
                    }
                }

                Text {
                    text: "Progress:"
                    color: Style.textColor
                }

                ProgressBar {
                    id: flashProgress
                    indeterminate: true
                    Layout.columnSpan:  4
                    Layout.fillWidth: true
                    value: 0.0
                    from: 0.0
                    to: 100.0
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        visible: false
        property var fileName: ""
        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrls)
            var sizeToRemove = fileDialog.folder.toString().length - fileDialog.fileUrl.toString().length + 1
            fileName = fileDialog.fileUrl.toString().slice(sizeToRemove)
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    Settings {
        property alias plotThemeIndex: plotThemeCB.currentIndex
        property alias replayItemChecked: replayChB.checked
        property alias smoothDataState: smoothDataChB.checkState
        property alias themeIndex: themeCB.currentIndex
    }

}
