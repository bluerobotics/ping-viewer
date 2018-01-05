import QtGraphicalEffects 1.0
import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Controls 1.4 as QC1
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

Item {
    id: mainPage
    visible: true

    PingItem {
        id: menuContainer
        anchors.left: parent.left
        anchors.top: parent.top
        item:  ColumnLayout {
            id: menu
            RowLayout {
            visible: true
                PingButton {
                    text: "Emit Ping"
                }

                Slider {
                    from: 0
                    value: 0
                    to: 100
                    onValueChanged: {
                        pingPerSecond.text = Math.floor(value).toString() + " ping/s"
                    }
                }

                Text {
                    id: pingPerSecond
                    text: "0 ping/s"
                }
            }
            RowLayout {

                PingButton {
                    text: "Auto Gain"
                }

                Slider {
                    from: 0.5
                    value: 0.5
                    to: 100

                    onValueChanged: {
                        gainText.text = Math.floor(value).toString() + " dB"
                    }
                }

                Text {
                    id: gainText
                    text: "0 dB"
                }
            }
        }
    }

    ColumnLayout {
        id: mainColumn
        anchors.fill: parent
        spacing: 0
        RowLayout {
            id: topMenuBar
            anchors.left: parent.left
            anchors.right: parent.right

            Text {
                anchors.centerIn: parent
                text: "Blue Robotics Ping Viewer"
            }

            PingButton {
                anchors.right: parent.right
                text: "\\/"
                onClicked: {
                    stack.push(settingsPage)
                }
            }
        }

        Ping1DVisualizer {
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
