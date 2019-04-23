import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Extras 1.4
import QtQuick.Layouts 1.3

import DeviceManager 1.0

PingPopup {
    id: root

    closePolicy: Popup.NoAutoClose

    onVisibleChanged: {
        visible ? DeviceManager.startDetecting() : DeviceManager.stopDetecting()
    }

    Component.onCompleted: {
        open()
    }

    ColumnLayout {
        anchors.fill: parent
        StackView {
            id: stack
            clip: true
            initialItem: deviceManager
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        ColumnLayout {
            Button {
                text: stack.depth == 1 ? "Manual Connection" : "Device Manager"
                Layout.fillWidth: true
                onClicked: {
                    print(stack.depth)
                    if(stack.depth == 1) {
                        stack.push(
                            Qt.createComponent(
                                "qrc:/ConnectionMenu.qml"
                            ).createObject(stack, {
                                "ping": Qt.binding(function() { return DeviceManager.primarySensor}),
                                "width": Qt.binding(function() { return stack.width}),
                                "height": Qt.binding(function() { return stack.height})
                            })
                        )
                    } else {
                        stack.pop()
                    }
                }
            }
            Button {
                id: cancel
                text: "Cancel"
                Layout.fillWidth: true
                onClicked: root.close()
            }
        }
    }

    Component {
        id: deviceManager
        ColumnLayout {
            id: mainLayout
            spacing: 10
            anchors.fill: parent

            Image {
                id: pingName
                Layout.alignment: Qt.AlignHCenter
                width: Math.min(root.width, root.height)/4
                height: width
                source: "/imgs/br_icon.svg"
                fillMode: Image.PreserveAspectFit
                mipmap: true
                smooth: true
                antialiasing: true
                // TODO: maybe add an activated property to disable style change in PingImage
                sourceSize.width: height
                sourceSize.height: width

                RotationAnimator on rotation {
                    from: 0
                    to: 360
                    duration: 3000
                    running: true
                    loops: Animation.Infinite
                }
            }

            Label {
                id: waitingText
                font.pixelSize: 18
                Layout.alignment: Qt.AlignHCenter
                text: "Waiting for devices..."
            }

            Timer {
                interval: 4000
                running: true
                repeat: true
                onTriggered: pathView.incrementCurrentIndex()
            }

            PathView {
                id: pathView
                Layout.fillWidth: true
                height: waitingText.font.pixelSize*2
                clip: true
                model: ["Searching serial devices..", "Doing underwater exploration..", "Looking local network..", "Talking with some dolphins.."]
                highlightMoveDuration: 1000
                interactive: false
                path: Path {
                    startX: -pathView.width
                    startY: 0
                    PathLine { x: -pathView.width*2; y: 0 }
                    PathLine { x: pathView.width*4; y: 0 }
                }
                delegate: Label {
                    anchors.top: pathView.top
                    horizontalAlignment: Text.AlignHCenter
                    width: pathView.width
                    text: modelData
                }
            }

            ListView {
                id: list
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: DeviceManager

                delegate: Rectangle {
                    height: 40
                    width: parent.width
                    border.width: 1
                    border.color: "gray"
                    color: "transparent"

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            DeviceManager.connectLink(connection)
                            // We should check if Sensor device is created and if it's really connected and
                            // communicating with the device
                            root.close()
                        }
                        ToolTip {
                            visible: parent.containsMouse
                            text: "Connect."
                        }
                    }

                    RowLayout {
                        anchors.fill: parent

                        AnimatedImage {
                            id: gif
                            Layout.preferredHeight: 25
                            Layout.preferredWidth: 25
                            Layout.leftMargin: 5
                            Layout.alignment: Qt.AlignLeft
                            mipmap: true
                            smooth: true
                            antialiasing: true
                            source: "/gifs/" + name + ".gif"
                        }

                        Label {
                            Layout.leftMargin: 5
                            Layout.alignment: Qt.AlignLeft
                            text: name
                        }

                        Label {
                            Layout.fillWidth: true
                            horizontalAlignment: Text.AlignHCenter
                            text: connection.typeToString() + " " + connection.createConfString()
                        }

                        StatusIndicator {
                            Layout.preferredHeight: 20
                            Layout.preferredWidth: 20
                            Layout.rightMargin: 5
                            Layout.alignment: Qt.AlignRight
                            color: available ? "green" : "red"
                            active: true

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                ToolTip {
                                    visible: parent.containsMouse
                                    text: available ? "Device available." : "Device busy or not available."
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
