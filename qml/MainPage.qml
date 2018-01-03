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
        id: menu
        z: mainLayout.z+1
        anchors.top: parent.top
        anchors.left: parent.left

        PingButton {
            text: "Menu"

            onClicked: {
                stack.push(settingsPage)
            }
        }

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
    }

    RowLayout {
        z: mainLayout.z+1
        anchors.top: parent.top
        anchors.right: parent.right

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

    ColumnLayout {
        id: mainLayout
        anchors.top: menu.bottom
        anchors.bottom: mainPage.bottom
        anchors.right: mainPage.right
        anchors.left: mainPage.left

        QC1.SplitView {
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
            GradientStop { position: 0.0; color: "#200000FF" }
            GradientStop { position: 1.0; color: "transparent" }
        }
    }
}