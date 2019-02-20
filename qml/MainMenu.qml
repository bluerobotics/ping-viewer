import QtGraphicalEffects 1.0
import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

import Ping1DNamespace 1.0
import SettingsManager 1.0
import StyleManager 1.0

Item {
    id: root
    visible: false
    height: settingsLayout.height
    width: settingsLayout.width
    property var ping: null

    ColumnLayout {
        id: settingsLayout
        GroupBox {
            id: pingGroup
            title: "Ping"
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
                        text: "Emit Ping"
                        //requestEchosounderProfile
                        onClicked: ping.request(Ping1DNamespace.Profile)
                    }

                    Slider {
                        id: pingHzSlider
                        from: 0
                        stepSize: 1
                        to: SettingsManager.debugMode ? ping.pingMaxFrequency : 30
                        value: ping.pingFrequency
                        Layout.fillWidth: true
                        onValueChanged: {
                            if (ping.pingFrequency !== value) {
                                ping.pingFrequency = value
                            }
                        }
                    }

                    Text {
                        id: pingPerSecond
                        text: Math.round(ping.pingFrequency) + " ping/s"
                        color: Material.primary
                    }
                }

                RowLayout {
                    spacing: 2

                    PingTextField {
                        id: speedOfSound
                        title: "Speed of Sound (m/s):"
                        text: ping.speed_of_sound / 1000
                        validator: DoubleValidator {
                            // Values in m/s
                            bottom: SettingsManager.debugMode ? 0 : 1400
                            top: SettingsManager.debugMode ? 1e5 : 1600
                        }
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        onEditingFinished: {
                            var speed_of_sound = parseFloat(text)
                            ping.speed_of_sound = speed_of_sound * 1000 // mm/s
                        }
                    }

                    Switch {
                        text: "Ping Enabled:"
                        Layout.columnSpan: 1
                        checked: ping.pingEnable
                        onCheckedChanged: ping.pingEnable = checked
                        contentItem.anchors.right: indicator.left
                        contentItem.anchors.margins: spacing
                        indicator.x: width - indicator.width - rightPadding
                    }


                    PingImage {
                        id: advancedSettingsButton
                        source: StyleManager.configureIcon()
                        height: 50
                        width: 50
                        selected: SettingsManager.enableSensorAdvancedConfiguration
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                advancedSettingsButton.selected = !advancedSettingsButton.selected
                                SettingsManager.enableSensorAdvancedConfiguration = advancedSettingsButton.selected
                            }
                        }
                    }

                    PingImage {
                        id: firmwareUpdateButton
                        source: StyleManager.chipIcon()
                        height: 50
                        width: 50
                        selected: false
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                firmwareUpdateButton.selected = !firmwareUpdateButton.selected
                                firmwareUpdate.visible = firmwareUpdateButton.selected
                            }
                        }
                    }
                }

                RowLayout {
                    spacing: 5
                    visible: SettingsManager.enableSensorAdvancedConfiguration


                    CheckBox {
                        id: autoGainChB
                        text: "Auto Gain:"
                        checked: ping.mode_auto
                        onCheckedChanged: {
                            ping.mode_auto = checked
                        }
                        contentItem.anchors.right: indicator.left
                        contentItem.anchors.margins: spacing
                        indicator.x: width - indicator.width - rightPadding - spacing
                    }

                    Label {
                        text: "Current Gain Setting:"
                        enabled: !autoGainChB.checked
                        color: Material.accent
                    }

                    ComboBox {
                        id: gainCB
                        currentIndex: ping.gain_index ? ping.gain_index : 0
                        model: [-4.4, 5.2, 14.8, 22.2, 29.6, 36.4, 43.2]
                        enabled: !autoGainChB.checked
                        Layout.columnSpan:  1
                        onCurrentIndexChanged: {
                            displayText = model[currentIndex]
                        }
                        onActivated: {
                            ping.gain_index = currentIndex
                        }
                    }
                }

                RowLayout {
                    spacing: 5
                    enabled: !autoGainChB.checked
                    visible: SettingsManager.enableSensorAdvancedConfiguration

                    property var maxDepthMm: SettingsManager.debugMode ? 1e6 : 5e4
                    property var minLengthMm: 1e3

                    function setDepth(startMm, lengthMm) {
                        // Sensor max range is limited to maxDepthMm
                        // The minimum range between start and length is minLengthMm

                        // Start
                        if(ping.start_mm !== startMm) {
                            // Check if length + new start is bigger than our limit (maxDepthMm)
                            if(startMm >= maxDepthMm - minLengthMm) {
                                startMm = maxDepthMm - minLengthMm
                            }

                            // Limit window to maxDepthMm
                            if(startMm + ping.length_mm > maxDepthMm) {
                                lengthMm = maxDepthMm - startMm
                            }

                            // Set new value
                            ping.start_mm = startMm
                        }

                        // Length
                        if(ping.length_mm !== lengthMm) {
                            // Check if length is less than minLengthMm
                            var lengthMm = lengthMm
                            if(lengthMm < minLengthMm) {
                                lengthMm = minLengthMm
                            }

                            // Check if length is inside our maxDepthMm
                            lengthMm = Math.min(lengthMm, maxDepthMm - ping.start_mm)

                            // If length is invalid, set minLengthMm
                            if(isNaN(lengthMm)) {
                                lengthMm = minLengthMm
                            }

                            // Set new value
                            ping.length_mm = lengthMm
                        }
                    }



                    PingTextField {
                        id: startLength
                        title: "Scan start point (mm):"
                        text: ping.start_mm
                        validator: IntValidator {
                            bottom: 0
                            top: maxDepthMm
                        }
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        onEditingFinished: {
                            parent.setDepth(parseInt(text), ping.length_mm)
                        }
                    }

                    PingTextField {
                        id: totalLength
                        title: "Length (mm):"
                        text: ping.length_mm
                        validator: IntValidator {
                            bottom: 0
                            top: maxDepthMm
                        }
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        onEditingFinished: {
                            parent.setDepth(ping.start_mm, parseInt(text))
                        }
                    }
                }
            }
        }

        FirmwareUpdate {
            id: firmwareUpdate
            visible: false
            ping: root.ping
        }
    }
}
