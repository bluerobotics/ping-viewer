import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Item {
    id: root
    anchors.fill: parent

    property var marginPix: 10
    property var ping: null

    Rectangle {
        id: rect
        color: "black"
        opacity: 0.75
        height: innerCol.height + 2 * marginPix
        width: innerCol.width + 5 * marginPix
        x: root.width / 2
        y: root.height / 2

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

            Repeater {
                model: [
                    "FW: " + ping.firmware_version_major + "." + ping.firmware_version_minor,
                    "SRC: " + ping.srcId + " DST: " + ping.dstId,
                    "Connection: " + ping.link.configuration.string,
                    "Distance (mm): " + ping.distance,
                    "Auto (bool): " + ping.mode_auto,
                    "Scan Start (mm): " + ping.start_mm,
                    "Scan Length (mm): " + ping.length_mm,
                    "Ping (#): " + ping.ping_number,
                    "Transmit duration (μs): " + ping.transmit_duration,
                    "Ping interval (ms): " + ping.ping_interval,
                    "Gain (setting): " + ping.gain_setting,
                    "Confidence (%): " + ping.confidence,
                    "Speed of sound (mm/s): " + ping.speed_of_sound,
                    "Processor temperature (C): " + (ping.processor_temperature / 100).toFixed(1),
                    "PCB temperature (C): " + (ping.pcb_temperature / 100).toFixed(1),
                    "Board voltage (V): " + (ping.board_voltage / 1000).toFixed(2),
                    "Lost messages (#): " + ping.lost_messages,
                    "RX Packets (#): " + ping.parsed_msgs,
                    "RX Errors (#): " + ping.parser_errors,
                    "Ascii text:\n" + ping.ascii_text,
                    "Error message:\n" + ping.err_msg,
                ]
                Text {
                    text: modelData
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
        }
    }
}
