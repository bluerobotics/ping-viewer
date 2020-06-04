import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

PingGroupBox {
    id: root

    title: "No sensor"

    ColumnLayout {
        Label {
            text: "Please connect with a device:"
        }

        Button {
            text: "Device Manager"
            Layout.fillWidth: true
            onClicked: {
                deviceManagerViewer.open();
                root.parent.hideItem = true;
            }
        }

    }

}
