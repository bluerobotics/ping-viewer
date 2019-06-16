import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import DeviceManager 1.0
import Ping360Namespace 1.0
import SettingsManager 1.0
import StyleManager 1.0

Item {
    id: root
    visible: false
    height: childrenRect.height
    width: childrenRect.width
    property var ping: DeviceManager.primarySensor

    ColumnLayout {
        id: settingsLayout
        GroupBox {
            id: pingGroup
            title: "Ping360"
            enabled: ping.connected
            // Hack
            label.x: width/2 - label.contentWidth/2
            Layout.fillWidth: true

            ColumnLayout {
                spacing: 5
                width: parent.width

                RowLayout {
                    spacing: 5

                    PingButton {
                        text: "Reverse Step"
                        onClicked: ping.deltaStep(-1)
                    }

                    PingButton {
                        text: "Forward Step"
                        onClicked: ping.deltaStep(1)
                    }

                    Label {
                        text: "Gain:"
                    }
                    ComboBox {
                        id: gainChB
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        model: ["Low", "Normal", "High"]
                        onCurrentIndexChanged: ping.gain_setting = currentIndex
                    }
                }

                RowLayout {
                    PingTextField {
                        id: transmitDuration
                        title: "Transmit Duration (ms):"
                        text: ping.transmit_duration
                        validator: IntValidator {
                            bottom: 0
                            top: 1000
                        }
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        onEditingFinished: {
                            ping.transmit_duration = parseInt(text)
                        }
                    }

                    PingTextField {
                        id: samplePeriod
                        title: "Sample Period (ms):"
                        // The sensor uses 25ns steps
                        text: ping.sample_period/40
                        validator: IntValidator {
                            bottom: 2
                            top: 1000
                        }
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        onEditingFinished: {
                            ping.sample_period = parseInt(text)*40
                        }
                    }
                }

                RowLayout {
                     PingTextField {
                        id: transmitFrequency
                        title: "Transmit Frequency (kHz):"
                        text: ping.transmit_frequency
                        validator: IntValidator {
                            bottom: 500
                            top: 1000
                        }
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        onEditingFinished: {
                            ping.transmit_frequency = parseInt(text)
                        }
                    }
                }
            }
        }
    }
}
