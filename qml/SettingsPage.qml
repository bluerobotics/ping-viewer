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

    property var plotThemes
    property var plotTheme: plotThemeCB.displayText

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
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                    }

                    Text {
                        text: "Communication:"
                        color: 'linen'
                    }

                    ComboBox {
                        displayText: "Serial (default)"
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                    }

                    Text {
                        text: "Serial Port / Baud:"
                        color: 'linen'
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
                        color: 'linen'
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
                        color: 'linen'
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
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
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
                        model: plotThemes
                    }

                    Text {
                        text: "Theme:"
                        color: 'linen'
                    }

                    ComboBox {
                        displayText: "Dark"
                        Layout.columnSpan:  4
                        Layout.fillWidth: true
                    }

                    CheckBox {
                        text: "Enable Advanced Mode"
                        checked: false
                        Layout.columnSpan:  5
                        Layout.fillWidth: true
                    }

                    CheckBox {
                        text: "Smooth Data"
                        checked: true
                        Layout.columnSpan:  5
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
        property alias plotThemeIndex: plotThemeCB.currentIndex
    }

}