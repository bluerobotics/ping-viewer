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
                ColumnLayout {
                    anchors.fill: parent
                    ColumnLayout {
                        RowLayout {
                            Text {
                                text: "Sonar Type:"
                            }

                            ComboBox {
                                displayText: "Ping Echosounder"
                            }
                        }
                    }

                    ColumnLayout {
                        RowLayout {
                            Text {
                                text: "Communication:"
                            }

                            ComboBox {
                                displayText: "Serial (default)"
                            }
                        }
                    }

                    ColumnLayout {
                        RowLayout {
                            Text {
                                text: "Serial Port / Baud:"
                            }

                            ComboBox {
                                displayText: ""
                            }

                            ComboBox {
                                displayText: "921600"
                            }
                        }
                    }

                    ColumnLayout {
                        RowLayout {
                            Text {
                                text: "UDP Host/Port:"
                            }

                            TextField {
                            }

                            TextField {
                            }
                        }
                    }

                    ColumnLayout {
                        RowLayout {
                            Text {
                                text: "Save Path:"
                            }

                            TextField {
                            }

                            PingButton {
                                text: "Browse..."
                            }
                        }
                    }
                }
            }

            GroupBox {
                id: displayGroup
                title: "Display"
                // Hack
                label.x: width/2 - label.contentWidth/2

                ColumnLayout {
                    RowLayout {
                        Text {
                            text: "Units:"
                        }

                        ComboBox {
                            displayText: "Meters"
                        }
                    }


                    RowLayout {
                        Text {
                            text: "Plot Theme:"
                        }

                        ComboBox {
                            displayText: "Thermal"
                        }
                    }

                    RowLayout {
                        Text {
                            text: "Theme:"
                        }

                        ComboBox {
                            displayText: "Dark"
                        }
                    }

                    CheckBox {
                        checked: false
                        text: "Enable Advanced Mode"
                    }

                    CheckBox {
                        checked: true
                        text: "Smooth Data"
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

            ColumnLayout {
            RowLayout {
                Text {
                    text: "Current Firmware:"
                }

                TextField {
                }
            }

            RowLayout {
                Text {
                    text: "Current Firmware:"
                }

                TextField {
                }
            }

            RowLayout {
                Text {
                    text: "Firmware File:"
                }

                TextField {
                }

                PingButton {
                    text: "Browse.."
                }
            }

            PingButton {
                text: "Firmware Update"
            }

            RowLayout {
                Text {
                    text: "Progress:"
                }

                ProgressBar {
                    indeterminate: true
                }
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