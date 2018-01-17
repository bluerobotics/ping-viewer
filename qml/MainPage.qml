import QtGraphicalEffects 1.0
import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0
import Ping 1.0

Item {
    id: mainPage
    visible: true

    Column {
        z: 1
        anchors.left: parent.left
        anchors.top: parent.top

        PingItem {
            id: menuContainer
            item:  GridLayout {
                columns: 5
                rowSpacing: 5
                columnSpacing: 5

                PingButton {
                    text: "Emit Ping"
                    onClicked: ping.protocol.requestEchosounderProfile()
                }

                Slider {
                    id: pingHzSlider
                    from: 0
                    value: 0
                    stepSize: 1
                    to: 35
                    Layout.columnSpan:  3
                    onValueChanged: {
                        pingPerSecond.text = Math.floor(value).toString() + " ping/s"
                        var period = 1000/value
                        if(isNaN(period) || period <= 0) {
                            pingTimer.stop()
                            return
                        }
                        pingTimer.start()
                        pingTimer.interval = period
                    }
                }

                Text {
                    id: pingPerSecond
                    text: "0 ping/s"
                    color: 'linen'
                }

                CheckBox {
                    id: autoGainChB
                    text: "Auto Gain"
                    onCheckedChanged: {
                        ping.protocol.setEchosounderAuto(checked)
                    }
                }

                ComboBox {
                    id: gainCB
                    model: [0.5, 1.4, 4.3, 10, 23.4, 71, 166, 338, 794, 1737]
                    enabled: !autoGainChB.checked
                    Layout.columnSpan:  4
                    Layout.fillWidth: true
                    onCurrentIndexChanged: {
                        displayText = model[currentIndex] + " dB"
                        ping.protocol.setEchosounderGain(currentIndex)
                    }
                }
            }
            onHideItemChanged: {
                if(hideItem == false) {
                    settingsMenu.hideItem = true
                    infoMenu.hideItem = true
                }
            }
        }

        PingItem {
            id: settingsMenu
            icon: "/icons/settings_white.svg"
            item: SettingsPage {
                id: settingsPage
                ping: ping
                waterfallItem: ping1DVisualizer.waterfallItem
            }
            onHideItemChanged: {
                if(hideItem == false) {
                    menuContainer.hideItem = true
                    infoMenu.hideItem = true
                }
            }
        }

        PingItem {
            id: infoMenu
            icon: "/icons/info_white.svg"
            item: InfoPage {}
            onHideItemChanged: {
                if(hideItem == false) {
                    menuContainer.hideItem = true
                    settingsMenu.hideItem = true
                }
            }
        }
    }

    Ping {
        id: ping
        onConnectionOpen: {
            ping.protocol.requestEchosounderMode()
            ping.protocol.requestEchosounderProfile()
        }
    }

    Connections {
        target: ping.protocol
        onEchosounderPoints: {
            ping1DVisualizer.draw(points)
        }

        onEchosounderDistance: {
            ping1DVisualizer.setDepth(mm/1e3)
        }

        onEchosounderGain: {
            gainCB.currentIndex = gain
        }

        onEchosounderAuto: {
            autoGainChB.checked = mode
        }
    }

    Timer {
        id: pingTimer
        interval: 500; running: false; repeat: true
        onTriggered: ping.protocol.requestEchosounderProfile()
    }

    ColumnLayout {
        id: mainColumn
        anchors.fill: parent
        spacing: 0
        Ping1DVisualizer {
            id: ping1DVisualizer
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    LinearGradient {
        anchors.fill: parent
        z: parent.z-1
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#FF11b3ff" }
            GradientStop { position: 1.0; color: "#FF111363" }
        }
    }
}
