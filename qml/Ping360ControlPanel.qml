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

            PingSlider {
                text: "Sector Angle (degrees)"
                value: ping.sectorSize
                control.from: 30
                control.to: 360
                control.stepSize: 15
                control.snapMode: Slider.SnapAlways
                control.onMoved: ping.sectorSize = control.value
            }
            PingSlider {
                Layout.fillWidth: true
                text: "Range (m)"
                value: Math.round(ping.range)
                control.stepSize: 1
                control.from: 1
                control.to: 100
                control.onMoved: ping.range = control.value
            }
            PingSlider {
                Layout.fillWidth: true
                text: "Receiver Gain"
                value: ping.gain_setting
                control.from: 0
                control.to: 2
                control.onMoved: ping.gain_setting = control.value
            }

            Rectangle { height: 1; Layout.fillWidth: true }

            PingSlider {
                Layout.fillWidth: true
                // TODO: proper symbol for us
                text: "Transmit Duration (μs)"
                value: ping.transmit_duration
                control.from: 5
                control.to: 1000
                control.onMoved: ping.transmit_duration = control.value
            }
            PingSlider {
                Layout.fillWidth: true
                text: "Transmit Frequency (kHz)"
                value: ping.transmit_frequency
                control.from: 500
                control.to: 1000
                control.onMoved: ping.transmit_frequency = control.value
            }
            PingSlider {
                Layout.fillWidth: true
                text: "Speed of Sound (m/s)"
                value: ping.speed_of_sound
                control.from: 1450
                control.to: 1550
                control.onMoved: ping.speed_of_sound = control.value
            }
            PingSlider {
                Layout.fillWidth: true
                text: "Angular Resolution (gradian)"
                value: ping.angular_speed
                control.from: 1
                control.to: 10
                control.onMoved: ping.angular_speed = control.value
            }
            PingSlider {
                Layout.fillWidth: true
                text: "Angle Offset (degree)"
                value: ping.angle_offset
                control.from: 0
                control.to: 359
                control.onMoved: ping.angle_offset = control.value
            }
        }
    }
}
