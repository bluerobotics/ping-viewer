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
                        validator: DoubleValidator {
                            // Values in m/s
                            bottom: SettingsManager.debugMode ? 0 : 1400
                            top: SettingsManager.debugMode ? 1e5 : 1600
                        }
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        onEditingFinished: {
                            var speed_of_sound = parseFloat(speedOfSound.text)
                            ping.speed_of_sound = speed_of_sound * 1000 // mm/s
                        }
                        Connections {
                            target: ping
                            onSpeedOfSoundUpdate: {
                                if (!speedOfSound.focus) {
                                    speedOfSound.text = ping.speed_of_sound / 1000
                                }
                            }
                        }
                    }

                    Switch {
                        text: "Ping Enabled"
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
                    enabled: !autoGainChB.checked
                    visible: SettingsManager.enableSensorAdvancedConfiguration


                    CheckBox {
                        id: autoGainChB
                        text: "Auto Gain"
                        checked: ping.mode_auto
                        onCheckedChanged: {
                            ping.mode_auto = checked
                        }
                    }

                    ComboBox {
                        id: gainCB
                        currentIndex: ping.gain_index ? ping.gain_index : 0
                        model: [-4.4, 5.2, 14.8, 22.2, 29.6, 36.4, 43.2]
                        enabled: !autoGainChB.checked
                        Layout.columnSpan:  1
                        Layout.fillWidth: true
                        onCurrentIndexChanged: {
                            displayText = model[currentIndex]
                        }
                        onActivated: {
                            ping.gain_index = currentIndex
                        }
                    }

                RowLayout {
                    spacing: 5
                    enabled: !autoGainChB.checked
                    visible: SettingsManager.enableSensorAdvancedConfiguration

                    PingTextField {
                        id: startLength
                        title: "Scan start point (mm):"
                        validator: IntValidator {
                            bottom: 0
                            top: SettingsManager.debugMode ? 1e6 : 7e5
                        }
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        onEditingFinished: {
                            var start_mm = parseInt(text)
                            text = start_mm
                            ping.start_mm = start_mm
                        }
                        Connections {
                            target: ping
                            onScanStartUpdate: {
                                if (!startLength.focus) {
                                    startLength.text = ping.start_mm
                                }
                            }
                        }
                    }

                    PingTextField {
                        id: totalLength
                        title: "Length (mm):"
                        validator: IntValidator {
                            bottom: 0
                            top: SettingsManager.debugMode ? 1e6 : 7e5
                        }
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        onEditingFinished: {
                            var start_mm = parseInt(startLength.text)
                            var length_mm = Math.min(parseInt(text), 50000 - start_mm)
                            if(isNaN(length_mm)) {
                                length_mm = 500
                            }
                            text = length_mm
                            ping.length_mm = length_mm
                        }
                        Connections {
                            target: ping
                            onScanLengthUpdate: {
                                if (!totalLength.focus) {
                                    totalLength.text = ping.length_mm
                                }
                            }
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
