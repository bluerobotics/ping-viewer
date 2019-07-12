import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import DeviceManager 1.0
import SettingsManager 1.0
import StyleManager 1.0

Item {
    id: root
    visible: false
    height: childrenRect.height
    width: childrenRect.width
    property var ping: DeviceManager.primarySensor

    GroupBox {
        id: pingGroup
        title: "Ping360"
        enabled: ping.connected
        // Hack
        label.x: width/2 - label.contentWidth/2
        Layout.fillWidth: true
        ColumnLayout {
            Layout.fillWidth: true
            RowLayout {
                Layout.fillWidth: true
                spacing: 5

                Label {
                    text: "Gain:"
                }
                PingComboBox {
                    id: gainChB
                    Layout.fillWidth: true
                    currentIndex: ping.gain_setting
                    model: ["Low", "Normal", "High"]
                    onCurrentIndexChanged: ping.gain_setting = currentIndex
                }

                PingTextField {
                    id: transmitFrequency
                    title: "Transmit Frequency (kHz):"
                    text: ping.transmit_frequency
                    validator: IntValidator {
                        bottom: 500
                        top: 1000
                    }
                    onEditingFinished: {
                        ping.transmit_frequency = parseInt(text)
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 5

                PingTextField {
                    title: "Angle Offset:"
                    text: ping.angle_offset
                    enabled: true
                    Layout.fillWidth: true
                    validator: IntValidator {
                        bottom: 0
                        top: 359
                    }
                    onEditingFinished: {
                        ping.angle_offset = parseInt(text)
                    }
                }

                PingTextField {
                    title: "Transmit Duration (ms):"
                    text: ping.transmit_duration
                    validator: IntValidator {
                        bottom: 1
                        top: 1000
                    }
                    onEditingFinished: {
                        ping.transmit_duration = parseInt(text)
                    }
                }
            }

            // TODO: Create ping switch style
            Switch {
                text: "Reverse direction:"
                Layout.columnSpan: 1
                checked: ping.reverse_direction
                onCheckedChanged: ping.reverse_direction = checked
                contentItem.anchors.right: indicator.left
                contentItem.anchors.margins: spacing
                indicator.x: width - indicator.width - rightPadding*2
            }

            Rectangle { height: 1; Layout.fillWidth: true }

            RowLayout {
                id: scanModeLayout
                Layout.fillWidth: true
                spacing: 5

                Label {
                    text: "Scan mode:"
                }
                PingComboBox {
                    id: modeComboBox
                    //TODO: Need to be done
                    enabled: false
                    model: ["Full", "Manual", "180", "90", "60", "45", "30"]
                    Layout.fillWidth: true
                }

                PingTextField {
                    title: "Speed (gradian):"
                    text: ping.angular_speed
                    validator: IntValidator {
                        bottom: 1
                        top: 50
                    }
                    onEditingFinished: {
                        ping.angular_speed = parseInt(text)
                    }
                }
            }

            RowLayout {
                id: scanModeConfLayout
                Layout.fillWidth: true
                enabled: false
                spacing: 5

                PingTextField {
                    title: "Central Angle(º):"
                    text: "0"
                    validator: IntValidator {
                        bottom: 0
                        top: 359
                    }
                    Layout.fillWidth: true
                }

                PingTextField {
                    title: "Angular Size (º):"
                    text: "60"
                    visible: modeComboBox.currentIndex == 1
                    validator: IntValidator {
                        bottom: 1
                        top: 270
                    }
                }
            }

            Rectangle { height: 1; Layout.fillWidth: true }

            RowLayout {
                Layout.fillWidth: true
                spacing: 5

                PingTextField {
                    title: "Length (mm):"
                    text: ping.length_mm
                    validator: IntValidator {
                        bottom: 1e3
                        top: (1e3)*1e2
                    }
                    Layout.fillWidth: true
                    onEditingFinished: {
                        ping.length_mm = parseInt(text)
                    }
                }

                PingTextField {
                    title: "Samples (#):"
                    text: ping.number_of_points
                    validator: IntValidator {
                        bottom: 200
                        top: 1200
                    }
                    onEditingFinished: {
                        ping.number_of_points = parseInt(text)
                    }
                }
            }
        }
    }
}
