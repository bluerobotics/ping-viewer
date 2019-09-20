import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQml.Models 2.12

import DeviceManager 1.0

Item {
    id: root
    anchors.fill: parent

    property var marginPix: 10
    property var ping: DeviceManager.primarySensor
    property var sensorModel: ping.sensorStatusModel(root)

    DelegateModel {
        id: baseModel
        property var title: "Base data:"

        model: ping ? [
            "FW: " + ping.firmware_version_major + "." + ping.firmware_version_minor,
            "SRC: " + ping.srcId + " DST: " + ping.dstId,
            "Device type: " + ping.device_type,
            "Device Revision: " + ping.device_revision,
            "Connection: " + ping.link.configuration.string,
            "RX Packets (#): " + ping.parsed_msgs,
            "RX Errors (#): " + ping.parser_errors,
            "Lost messages (#): " + ping.lost_messages,
            "Ascii text:\n" + ping.ascii_text,
            "Error message:\n" + ping.err_msg,
        ] : []

        delegate: Text {
            text: modelData
            color: "white"
            font.pointSize: 8
        }
    }

    Rectangle {
        id: rect
        color: "black"
        opacity: 0.75
        height: innerCol.height + 2 * marginPix
        width: innerCol.width + 5 * marginPix

        MouseArea {
            anchors.fill: parent
            drag.target: rect
            drag.minimumX: 0
            drag.minimumY: 0
            drag.maximumX: root.width - rect.width
            drag.maximumY: root.height - rect.height
        }

        ColumnLayout {
            id: innerCol
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: marginPix

            Label {
                text: baseModel.title
            }
            //We disable the models with visibility to avoid unnecessary CPU usage in realocation
            //TODO: We should improve it to remove the amount of CPU and realocation when visible
            Repeater {
                model: root.visible ? baseModel : []
            }

            Label {
                text: sensorModel.title
            }
            Repeater {
                model: root.visible ? sensorModel.model : []
            }
        }

        Component.onCompleted: {
            x = root.width / 2
            y = root.height / 2
        }
    }

    // Take care of the item position while resizing parent
    property var parentWidth: null
    property var parentHeight: null
    Connections {
        target: root.parent
        onWidthChanged: {
            if (parentWidth) {
                var scaleW = root.parent.width/parentWidth
                if (scaleW) {
                    rect.x *= scaleW
                    parentWidth = root.parent.width
                }
                return
            }
            parentWidth = root.parent.width
        }
        onHeightChanged: {
            if (parentHeight) {
                var scaleH = root.parent.height/parentHeight
                if (scaleH) {
                    rect.y *= scaleH
                    parentHeight = root.parent.height
                }
                return
            }
            parentHeight = root.parent.height
        }
    }
}
