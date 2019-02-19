import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Item {
    id: root
    anchors.fill: parent

    property var marginPix: 10
    property var ping

    Rectangle {
        anchors.centerIn: parent

        color: "black"
        opacity: 0.75
        height: innerCol.height + 2 * marginPix
        width: innerCol.width + 5 * marginPix
        ColumnLayout {
            id: innerCol
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: marginPix

            Repeater {
                model: [
                    "FW: " + ping.firmware_version_major + "." + ping.firmware_version_minor,
                    "SRC: " + ping.srcId + " DST: " + ping.dstId,
                    "Connection: " + ping.link.configuration.createConfString(),
                    "Distance (mm): " + ping.distance,
                    "Auto (bool): " + ping.mode_auto,
                    "Scan Start (mm): " + ping.start_mm,
                    "Scan Length (mm): " + ping.length_mm,
                    "Ping (#): " + ping.ping_number,
                    "Pulse duration (μs): " + ping.pulse_duration,
                    "Ping interval (ms): " + ping.ping_interval,
                    "Gain (index): " + ping.gain_index,
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
