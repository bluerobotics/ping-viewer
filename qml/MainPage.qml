import QtGraphicalEffects 1.0
import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Controls 1.4 as QC1
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

Item {
    id: mainPage
    visible: true

    RowLayout {
        id: mainLayout
        z: mainColumn.z + 1
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        Rectangle {
            id: menuContainer
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: menu.width
            Layout.preferredHeight: menu.height
            visible: false
            color: 'white'

            Column {
                id: menu

                PingButton {
                    text: "Single"
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
                Text {
                    text: "Range/Gain"
                }

                PingButton {
                    text: "Auto"
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

        PingButton {
            id: pingButton
            anchors.bottom: parent.bottom
            text: menuContainer.visible ? "<" : ">"
            onClicked: {
                menuContainer.visible = !menuContainer.visible
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

        QC1.SplitView {
            id: visualizer
            Layout.fillHeight: true
            Layout.fillWidth: true
            orientation: Qt.Horizontal

            Waterfall {
                id: waterfall
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumWidth: 150
            }

            Chart {
                id: chart
                Layout.fillHeight: true
                Layout.preferredWidth: 350
                Layout.minimumWidth: 350
            }
        }
    }

    Settings {
        property alias chartWidth: chart.width
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
