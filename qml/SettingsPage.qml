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

    property var waterfallItem

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
                        enabled: false
                        color: 'linen'
                    }

                    ComboBox {
                        displayText: "Serial (default)"
                        enabled: false
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
                        enabled: false
                        Layout.columnSpan:  2
                        Layout.fillWidth: true
                    }

                    TextField {
                        enabled: false
                        Layout.columnSpan:  2
                        Layout.fillWidth: true
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
                        model: waterfallItem.themes
                        onCurrentTextChanged: {
                            waterfallItem.theme = currentText
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
                    }

                    CheckBox {
                        text: "Enable Advanced Mode"
                        enabled: false
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
        property alias plotThemeIndex: plotThemeCB.currentIndex
        property alias smoothDataState: smoothDataChB.checkState
    }

}