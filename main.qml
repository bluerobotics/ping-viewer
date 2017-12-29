import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0
import "."

Window {
    id: window

    RowLayout{
        anchors.fill : parent
        Waterfall {
            id: waterfall
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        Chart {
            id: c
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    Settings {
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
    }

    RowLayout {
        id: buttonLayout
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        opacity: 0.5

        PingButton {
            id: bt1
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "-----"
            onClicked: {
            }
        }

        PingButton {
            id: bt2
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "-----"
            onClicked: {
            }
        }

        PingButton {
            id: bt3
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "-----"
            onClicked: {
            }
        }

        PingButton {
            id: bt4
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "-----"
            onClicked: {
            }
        }
    }
}