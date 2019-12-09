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

        PingSlider {
            id: rangeSlider
            Layout.fillWidth: true
            text: "Range (m)"
            model: [1, 2, 5, 10, 15, 20, 30, 40, 50, 60]
            modelValue: ping.range

            Binding {
                target: ping
                property: "range"
                value: rangeSlider.modelValue
            }
        }

        PingSlider {
            id: gainSlider
            text: "Receiver Gain"
            Layout.fillWidth: true
            model: ["Low", "Medium", "High"]

            Binding {
                target: ping
                property: "gain_setting"
                value: gainSlider.modelIndex
            }
            Binding {
                target: gainSlider
                property: "modelIndex"
                value: ping.gain_setting
            }
        }

        PingSlider {
            id: sectorSlider
            Layout.fillWidth: true
            text: "Sector Angle (degrees)"
            model: [60, 90, 120, 180, 360]

            Binding {
                target: ping
                property: "sectorSize"
                value: sectorSlider.modelValue
            }
            Binding {
                target: sectorSlider
                property: "modelValue"
                value: ping.sectorSize
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight
            PingButton {
                text: "Reset settings"
                Layout.fillWidth: true
                onClicked: ping.resetSettings()
            }

            PingImage {
                id: advancedSettingsButton
                source: StyleManager.arrowIcon()
                height: 50
                width: 50
                selected: false
                angle: 90
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        advancedSettingsButton.selected = !advancedSettingsButton.selected
                    }
                }

                onSelectedChanged: {
                    arrowRotation.from = selected ? 0 : angle + 90
                    arrowRotation.to = selected ? angle + 90 : 0
                    arrowRotation.running = true
                }

                RotationAnimator on rotation {
                    id: arrowRotation
                    duration: 200
                    direction: RotationAnimation.Shortest
                    running: false
                    easing.type: animationType
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
            RowLayout {
                visible: ping.link.type == AbstractLinkNamespace.Serial
                Button {
                    id: autoBaudRateChB
                    text: "Auto baudrate"
                    onClicked: ping.startConfiguration()
                }
                PingComboBox {
                    id: baudRateComboBox
                    model: ping.validBaudRates
                    Layout.fillWidth: true
                    enabled: !autoBaudRateChB.checked
                    onCurrentTextChanged: {
                        if(currentText != ping.link.configuration.serialBaudrate()) {
                            ping.setBaudRateAndRequestProfile(parseInt(currentText))
                        }
                    }

                    Connections {
                        target: ping
                        onLinkChanged: {
                            // Change baud rate in combobox if the one of the sensor changes
                            if(ping.link.configuration.serialBaudrate() == baudRateComboBox.currentText) {
                                return
                            }

                            var newIndex = baudRateComboBox.find(ping.link.configuration.serialBaudrate())
                            if(newIndex === -1) {
                                print("Baud rate is valid for this sensor.")
                                return
                            }

                            baudRateComboBox.currentIndex = newIndex
                        }
                    }
                }
            }
        }
    }
}
