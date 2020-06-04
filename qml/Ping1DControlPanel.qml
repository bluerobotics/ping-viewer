import DeviceManager 1.0
import QtGraphicalEffects 1.0
import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import SettingsManager 1.0
import StyleManager 1.0

ColumnLayout {
    id: root

    property var ping: DeviceManager.primarySensor

    PingGroupBox {
        id: pingGroup

        title: "Ping"
        enabled: ping.connected
        Layout.fillWidth: true

        ColumnLayout {
            spacing: 5
            width: parent.width

            RowLayout {
                spacing: 5

                PingButton {
                    text: "Emit Ping"
                    //requestEchosounderProfile
                    onClicked: ping.emitPing()
                }

                Slider {
                    id: pingHzSlider

                    from: 0
                    stepSize: 1
                    to: SettingsManager.debugMode ? ping.pingMaxFrequency : 30
                    value: ping.pingFrequency
                    Layout.fillWidth: true
                    onValueChanged: {
                        if (ping.pingFrequency !== value)
                            ping.pingFrequency = value;

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
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    onEditingFinished: {
                        var speed_of_sound = parseFloat(text);
                        ping.speed_of_sound = speed_of_sound * 1000; // mm/s
                    }

                    validator: DoubleValidator {
                        // Values in m/s
                        bottom: SettingsManager.debugMode ? 0 : 1400
                        top: SettingsManager.debugMode ? 100000 : 1600
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

                PingButton {
                    text: "Reset settings"
                    Layout.columnSpan: 1
                    onClicked: ping.resetSettings()
                }

                PingImage {
                    id: advancedSettingsButton

                    source: StyleManager.arrowIcon()
                    height: 50
                    width: 50
                    selected: SettingsManager.enableSensorAdvancedConfiguration
                    angle: 90
                    onSelectedChanged: {
                        arrowRotation.from = selected ? 0 : 180;
                        arrowRotation.to = selected ? 180 : 0;
                        arrowRotation.running = true;
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            advancedSettingsButton.selected = !advancedSettingsButton.selected;
                            SettingsManager.enableSensorAdvancedConfiguration = advancedSettingsButton.selected;
                        }
                    }

                    RotationAnimator on rotation {
                        id: arrowRotation

                        from: startAngle
                        to: startAngle
                        duration: 200
                        direction: RotationAnimation.Shortest
                        running: false
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
                        ping.mode_auto = checked;
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

                    currentIndex: ping.gain_setting ? ping.gain_setting : 0
                    model: [-4.4, 5.2, 14.8, 22.2, 29.6, 36.4, 43.2]
                    enabled: !autoGainChB.checked
                    Layout.columnSpan: 1
                    onCurrentIndexChanged: {
                        displayText = model[currentIndex];
                    }
                    onActivated: {
                        ping.gain_setting = currentIndex;
                    }
                }

            }

            RowLayout {
                // Sensor max range is limited to maxDepthMm
                // The minimum range between start and length is minLengthMm

                property var maxDepthMm: SettingsManager.debugMode ? 1e+06 : 50000
                property var minLengthMm: 1000

                function setDepth(startMm, lengthMm) {
                    // Start
                    if (ping.start_mm !== startMm) {
                        // Check if length + new start is bigger than our limit (maxDepthMm)
                        if (startMm >= maxDepthMm - minLengthMm)
                            startMm = maxDepthMm - minLengthMm;

                        // Limit window to maxDepthMm
                        if (startMm + ping.length_mm > maxDepthMm)
                            lengthMm = maxDepthMm - startMm;

                        // Set new value
                        ping.start_mm = startMm;
                    }
                    // Length
                    if (ping.length_mm !== lengthMm) {
                        // Check if length is less than minLengthMm
                        var internalLengthMm = lengthMm;
                        if (internalLengthMm < minLengthMm)
                            internalLengthMm = minLengthMm;

                        // Check if length is inside our maxDepthMm
                        internalLengthMm = Math.min(lengthMm, maxDepthMm - ping.start_mm);
                        // If length is invalid, set minLengthMm
                        if (isNaN(internalLengthMm))
                            internalLengthMm = minLengthMm;

                        // Set new value
                        ping.length_mm = internalLengthMm;
                    }
                }

                spacing: 5
                enabled: !autoGainChB.checked
                visible: SettingsManager.enableSensorAdvancedConfiguration

                PingTextField {
                    id: startLength

                    title: "Scan start point (mm):"
                    text: ping.start_mm
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    onEditingFinished: {
                        parent.setDepth(parseInt(text), ping.length_mm);
                    }

                    validator: IntValidator {
                        bottom: 0
                        top: startLength.parent.maxDepthMm
                    }

                }

                PingTextField {
                    id: totalLength

                    title: "Length (mm):"
                    text: ping.length_mm
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    onEditingFinished: {
                        parent.setDepth(ping.start_mm, parseInt(text));
                    }

                    validator: IntValidator {
                        bottom: 0
                        top: startLength.parent.maxDepthMm
                    }

                }

            }

        }

    }

}
