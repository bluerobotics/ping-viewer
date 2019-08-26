import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import AbstractLinkNamespace 1.0
import DeviceManager 1.0
import SettingsManager 1.0
import StyleManager 1.0

PingGroupBox {
    id: root
    title: "Ping360"
    enabled: ping.connected
    Layout.fillWidth: true
    property var ping: DeviceManager.primarySensor

    ColumnLayout {
        Layout.fillWidth: true

        PingComboSlider {
            Layout.fillWidth: true
            text: "Range (m)"
            model: [1, 2, 5, 10, 15, 20, 30, 40, 50, 60]
            onOptionChanged: ping.range = value
            startingValue: Math.round(ping.range)
        }

        PingComboSlider {
            text: "Receiver Gain"
            Layout.fillWidth: true
            startingIndex: ping.gain_setting
            model: ["Low", "Medium", "High"]
            onOptionChanged: {
                ping.gain_setting = key
            }
        }

        PingComboSlider {
            Layout.fillWidth: true
            text: "Sector Angle (degrees)"
            startingIndex: 5
            model: [60, 90, 120, 180, 360]
            onOptionChanged: {
                ping.sectorSize = value
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight
            PingImage {
                id: advancedSettingsButton
                source: StyleManager.configureIcon()
                height: 50
                width: 50
                selected: false
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        advancedSettingsButton.selected = !advancedSettingsButton.selected
                    }
                }
            }

            PingImage {
                id: firmwareUpdateButton
                source: StyleManager.chipIcon()
                height: 50
                width: 50
                enabled: false
                selected: false
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        firmwareUpdateButton.selected = !firmwareUpdateButton.selected
                    }
                }
            }
        }

        ColumnLayout {
            id: advancedSettings
            visible: advancedSettingsButton.selected
            PingSlider {
                id: transmitDurationSlider
                Layout.fillWidth: true
                text: "Transmit Duration (μs)"
                from: ping.transmitDurationMin
                to: ping.transmitDurationMax
                onValueChanged: ping.transmit_duration = value
                value: ping.transmit_duration
            }
            PingSlider {
                Layout.fillWidth: true
                text: "Transmit Frequency (kHz)"
                value: ping.transmit_frequency
                from: 500
                to: 1000
                onValueChanged: ping.transmit_frequency = value
            }
            PingSlider {
                Layout.fillWidth: true
                text: "Speed of Sound (m/s)"
                value: ping.speed_of_sound
                from: 1450
                to: 1550
                onValueChanged: ping.speed_of_sound = value
            }
            PingSlider {
                Layout.fillWidth: true
                text: "Angular Resolution (gradian)"
                value: ping.angular_speed
                from: 1
                to: 10
                onValueChanged: ping.angular_speed = value
            }
            PingSlider {
                Layout.fillWidth: true
                text: "Angle Offset (degree)"
                value: ping.angle_offset
                from: 0
                to: 359
                onValueChanged: ping.angle_offset = value
            }
            RowLayout {
                visible: ping.link.type == AbstractLinkNamespace.Serial
                Button {
                    id: autoBaudRateChB
                    text: "Auto baudrate"
                    onClicked: ping.startConfiguration()
                }
                PingComboBox {
                    model: ping.validBaudRates
                    Layout.fillWidth: true
                    enabled: !autoBaudRateChB.checked
                    onCurrentTextChanged: {
                        ping.setBaudRateAndRequestProfile(parseInt(currentText))
                    }
                }
            }
        }
    }
}
