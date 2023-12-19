import AbstractLinkNamespace 1.0
import DeviceManager 1.0
import LinkConfiguration 1.0
import PingEnumNamespace 1.0
import QtQuick 2.15
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Extras 1.4
import QtQuick.Layouts 1.3
import StyleManager 1.0

PingPopup {
    id: root

    enum ConnectionStatus {
        Available,
        Unavailable,
        InvalidConfiguration,
        ConfigurationIsRequired
    }

    closePolicy: Popup.NoAutoClose
    onVisibleChanged: {
        print(stack.depth);
        if (visible) {
            if (stack.depth == 1)
                DeviceManager.startDetecting();

        } else {
            DeviceManager.stopDetecting();
        }
    }
    Component.onCompleted: {
        if (!DeviceManager.primarySensor)
            open();

    }

    Connections {
        function onPrimarySensorChanged() {
            if (DeviceManager.primarySensor)
                close();

        }

        target: DeviceManager
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
                    print(stack.depth);
                    if (stack.depth == 1) {
                        DeviceManager.stopDetecting();
                        stack.push(connectionMenu);
                    } else {
                        DeviceManager.startDetecting();
                        stack.pop();
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
        id: connectionMenu

        ConnectionMenu {
            ping: DeviceManager.primarySensor
            width: stack.width
            height: stack.height
            onCloseRequest: {
                root.close();
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
                width: Math.min(root.width, root.height) / 4
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
                height: waitingText.font.pixelSize * 2
                clip: true
                model: ["Searching serial devices..", "Doing underwater exploration..", "Looking local network..", "Talking with some dolphins.."]
                highlightMoveDuration: 1000
                interactive: false

                path: Path {
                    startX: -pathView.width
                    startY: 0

                    PathLine {
                        x: -pathView.width * 2
                        y: 0
                    }

                    PathLine {
                        x: pathView.width * 4
                        y: 0
                    }

                }

                delegate: Label {
                    anchors.top: pathView.top
                    horizontalAlignment: Text.AlignHCenter
                    width: pathView.width
                    text: modelData
                }

            }

            Rectangle {
                height: 2
                Layout.fillWidth: true
                color: "gainsboro"
            }

            Label {
                id: noSensorLabel

                visible: list.count === 0
                Layout.fillWidth: true
                font.pixelSize: 20
                horizontalAlignment: Text.AlignHCenter
                text: "No devices detected"
            }

            ListView {
                id: list

                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: DeviceManager

                delegate: Rectangle {
                    id: rectDelegate

                    //TODO: Move this logic and variable to C++ somehow
                    property var connectionStatus: {
                        if (!connection.isValid())
                            return DeviceManagerViewer.ConnectionStatus.InvalidConfiguration;

                        if (connection.deviceType() == PingEnumNamespace.PingDeviceType.PING360 && connection.type() == AbstractLinkNamespace.Udp && connection.isSubnetBroadcast())
                            return DeviceManagerViewer.ConnectionStatus.ConfigurationIsRequired;

                        if (available)
                            return DeviceManagerViewer.ConnectionStatus.Available;

                        return DeviceManagerViewer.ConnectionStatus.Unavailable;
                    }

                    height: 40
                    width: parent.width
                    color: buttonMouseArea.containsMouse ? "gainsboro" : "transparent"

                    Rectangle {
                        height: 1
                        color: "gray"
                        visible: index < DeviceManager.count - 1

                        anchors {
                            left: rectDelegate.left
                            right: rectDelegate.right
                            top: rectDelegate.bottom
                        }

                    }

                    MouseArea {
                        id: buttonMouseArea

                        anchors.fill: parent
                        hoverEnabled: true
                        enabled: rectDelegate.connectionStatus != DeviceManagerViewer.ConnectionStatus.ConfigurationIsRequired
                        onClicked: {
                            DeviceManager.connectLink(connection);
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

                        // Use same layout if item is not visible, to have a proper alignment
                        Item {
                            height: parent.height
                            width: height

                            PingImage {
                                /**
                                 * We should only be visible when running under UDP connections with Ping360 and
                                 * not using bridges as an companion interface, since we can't ensure if it's a serial
                                 * connection
                                 */

                                id: iconConfiguration

                                source: StyleManager.settingsIcon()
                                anchors.fill: parent
                                selected: deviceConfigureMouseArea.containsMouse
                                Layout.alignment: Qt.AlignRight
                                visible: {
                                    const canBeConfigured = connection.deviceType() == PingEnumNamespace.PingDeviceType.PING360 && connection.type() == AbstractLinkNamespace.Udp && !connection.isCompanionPort();
                                    const configurationRequired = parent.parent.connectionStatus == DeviceManagerViewer.ConnectionStatus.ConfigurationIsRequired;
                                    return canBeConfigured || configurationRequired;
                                }

                                MouseArea {
                                    id: deviceConfigureMouseArea

                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: {
                                        root.close();
                                        ping360EthernetConfiguration.configureConnection(connection);
                                    }
                                }

                            }

                        }

                        StatusIndicator {
                            Layout.preferredHeight: 20
                            Layout.preferredWidth: 20
                            Layout.rightMargin: 5
                            Layout.alignment: Qt.AlignRight
                            color: {
                                switch (rectDelegate.connectionStatus) {
                                case DeviceManagerViewer.ConnectionStatus.Available:
                                    return "green";
                                case DeviceManagerViewer.ConnectionStatus.Unavailable:
                                    return "red";
                                case DeviceManagerViewer.ConnectionStatus.InvalidConfiguration:
                                    return "yellow";
                                case DeviceManagerViewer.ConnectionStatus.ConfigurationIsRequired:
                                    return "blue";
                                default:
                                    return "orange";
                                }
                            }
                            active: true

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true

                                ToolTip {
                                    visible: parent.containsMouse
                                    text: {
                                        switch (rectDelegate.connectionStatus) {
                                        case DeviceManagerViewer.ConnectionStatus.Available:
                                            return "Device available.";
                                        case DeviceManagerViewer.ConnectionStatus.Unavailable:
                                            return "Device busy or not available.";
                                        case DeviceManagerViewer.ConnectionStatus.InvalidConfiguration:
                                            return connection.errorToString();
                                        case DeviceManagerViewer.ConnectionStatus.ConfigurationIsRequired:
                                            return "Configuration is required.";
                                        default:
                                            return "Unknown connection status.";
                                        }
                                    }
                                }

                            }

                        }

                    }

                }

            }

        }

    }

}
