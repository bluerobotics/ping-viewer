import QtGraphicalEffects 1.0
import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Controls 1.4 as QC1
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

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
                    text: "Auto Gain"
                }

                Slider {
                    from: 0
                    value: 0
                    to: 100
                    Layout.columnSpan:  3
                    onValueChanged: {
                        gainText.text = Math.floor(value).toString() + " dB"
                    }
                }

                Text {
                    id: gainText
                    text: "0 dB"
                    color: 'linen'
                }


                PingButton {
                    text: "Emit Ping"
                }

                Slider {
                    from: 0
                    value: 0
                    to: 100
                    Layout.columnSpan:  3
                    onValueChanged: {
                        pingPerSecond.text = Math.floor(value).toString() + " ping/s"
                    }
                }

                Text {
                    id: pingPerSecond
                    text: "0 ping/s"
                    color: 'linen'
                }
            }
        }

        PingItem {
            id: settingsMenu
            icon: "/icons/settings_white.svg"
            item: SettingsPage {
                id: settingsPage
                waterfallItem: ping1DVisualizer.waterfallItem
            }
        }

        PingItem {
            id: infoMenu
            icon: "/icons/info_white.svg"
            item: InfoPage {
            }
        }
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
