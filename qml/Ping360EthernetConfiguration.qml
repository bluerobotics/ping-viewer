import DeviceManager 1.0
import Ping360HelperService 1.0
import QtQml.Models 2.2
import QtQuick 2.15
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Layouts 1.3
import StyleManager 1.0

PingPopup {
    id: root

    property var connection: undefined

    function configureConnection(connection) {
        open();
        root.connection = connection;
    }

    closePolicy: Popup.NoAutoClose
    // Default size
    width: 300
    height: 280

    Label {
        id: title

        text: "Ping360 Ethetnet configuration"
        anchors.horizontalCenter: parent.horizontalCenter
    }

    ListModel {
        id: ethernetModes

        ListElement {
            name: "DHCP Client"
            info: "Allows to connect as a normal ethernet client to a DHCP server."
        }

        ListElement {
            name: "Static IP"
            info: "Used to set a fixed IP address in a personalized network environment.<br>⚠ <strong>Ensure to configure your subnet if necessary</strong> ⚠"
        }

    }

    GridLayout {
        id: grid

        columns: 2
        anchors.top: title.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        RowLayout {
            Layout.columnSpan: 2
            Layout.fillWidth: true

            Label {
                text: "Type:"
            }

            PingComboBox {
                id: comboBox

                model: ethernetModes
                textRole: "name"
                Layout.fillWidth: true
            }

        }

        Text {
            text: ethernetModes.get(comboBox.currentIndex).info
            color: comboBox.currentIndex == 1 ? "red" : "black"
            horizontalAlignment: Text.AlignHCenter | Text.AlignJustify
            wrapMode: Text.Wrap
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        PingTextField {
            id: ipTextField

            title: "IP address:"
            text: "192.168.2.4"
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignHCenter
            visible: comboBox.currentIndex == 1

            // TODO: Move to RegularExpressionValidator when using Qt 5.14+
            // Check for valid IPV4 (0.0.0.0, 192.168.0.2)
            validator: RegExpValidator {
                regExp: /^(?!\.)((^|\.)([1-9]?\d|1\d\d|2(5[0-5]|[0-4]\d))){4}$/gm
            }

        }

        Button {
            id: confirmButton

            text: "Confirm"
            Layout.fillWidth: true
            onClicked: {
                let ip = connection.argsAsConst()[0];
                if (comboBox.currentIndex == 0)
                    Ping360HelperService.setDHCPServer(ip);
                else
                    Ping360HelperService.setStaticIP(ip, ipTextField.text);
                // Reset model to remove old results
                DeviceManager.clear();
                root.close();
                deviceManagerViewer.open();
            }
        }

        Button {
            id: cancelButton

            text: "Cancel"
            Layout.fillWidth: true
            onClicked: {
                root.close();
                deviceManagerViewer.open();
            }
        }

    }

}
