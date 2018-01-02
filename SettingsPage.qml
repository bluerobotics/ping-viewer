import QtGraphicalEffects 1.0
import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

Item {
    id: settingsPage
    visible: false

    RowLayout {
        id: confMenu
        anchors.top: parent.top
        anchors.left: parent.left

        PingButton {
            text: "Menu"

            onClicked: {
                stack.pop()
            }
        }
    }

    ColumnLayout {
        anchors.top: confMenu.bottom
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
                    }

                    ComboBox {
                        displayText: "Ping Echosounder"
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                    }


                    Text {
                        text: "Communication:"
                    }

                    ComboBox {
                        displayText: "Serial (default)"
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                    }

                    Text {
                        text: "Serial Port / Baud:"
                    }

                    ComboBox {
                        displayText: ""
                        Layout.columnSpan:  3
                        Layout.fillWidth: true
                    }

                    ComboBox {
                        id: baudrateBox
                        displayText: "921600"
                    }

                    Text {
                        text: "UDP Host/Port:"
                    }

                    TextField {
                        Layout.columnSpan:  2
                        Layout.fillWidth: true
                    }

                    TextField {
                        Layout.columnSpan:  2
                        Layout.fillWidth: true
                    }

                    Text {
                        text: "Save Path:"
                    }

                    TextField {
                        id: pathField
                        Layout.columnSpan:  3
                        Layout.fillWidth: true
                    }

                    Button {
                        text: "Browse..."
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

                GridLayout {
                    anchors.fill: parent
                    columns: 3
                    rowSpacing: 5
                    columnSpacing: 5

                    Text {
                        text: "Units:"
                    }

                    ComboBox {
                        displayText: "Meters"
                        Layout.columnSpan:  2
                        Layout.fillWidth: true
                    }

                    Text {
                        text: "Plot Theme:"
                    }

                    ComboBox {
                        displayText: "Thermal"
                        Layout.columnSpan:  2
                        Layout.fillWidth: true
                    }

                    Text {
                        text: "Theme:"
                    }

                    ComboBox {
                        displayText: "Dark"
                        Layout.columnSpan:  2
                        Layout.fillWidth: true
                    }

                    CheckBox {
                        text: "Enable Advanced Mode"
                        checked: false
                        Layout.columnSpan:  3
                        Layout.fillWidth: true
                    }

                    CheckBox {
                        text: "Smooth Data"
                        checked: true
                        Layout.columnSpan:  3
                        Layout.fillWidth: true
                    }
                }
            }
        }

        GroupBox {
            id: firmwareGroup
            title: "Firmware Update"
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
                }

                TextField {
                    Layout.columnSpan:  4
                    Layout.fillWidth: true
                }

                Text {
                    id: firmwareLabel
                    text: "Firmware File:"
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
                }

                ProgressBar {
                    indeterminate: true
                    Layout.columnSpan:  4
                    Layout.fillWidth: true
                }
            }
        }
    }

    LinearGradient {
        anchors.fill: parent
        z: parent.z-1
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#200000FF" }
            GradientStop { position: 1.0; color: "transparent" }
        }
    }
}
