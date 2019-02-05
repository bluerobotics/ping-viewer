import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4 as QC1
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
            Text {
                text: "FW: " + ping.firmware_version_major + "." + ping.firmware_version_minor
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "SRC: " + ping.srcId + " DST: " + ping.dstId
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "Connection: " + ping.link.configuration.createConfString()
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "Distance (mm): " + ping.distance
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "Auto (bool): " + ping.mode_auto
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "Scan Start (mm): " + ping.start_mm
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "Scan Length (mm): " + ping.length_mm
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "Ping (#): " + ping.ping_number
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "Pulse duration (μs): " + ping.pulse_duration
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "Ping interval (ms): " + ping.ping_interval
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "Gain (index): " + ping.gain_index
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "Confidence (%): " + ping.confidence
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "Speed of sound (mm/s): " + ping.speed_of_sound
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "Processor temperature (C): " + (ping.processor_temperature / 100).toFixed(1)
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "PCB temperature (C): " + (ping.pcb_temperature / 100).toFixed(1)
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "Board voltage (V): " + (ping.board_voltage / 1000).toFixed(2)
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "Lost messages (#): " + ping.lost_messages
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "RX Packets (#): " + ping.parsed_msgs
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "RX Errors (#): " + ping.parser_errors
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "Ascii text:\n" + ping.ascii_text
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "Error message:\n" + ping.err_msg
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
            Text {
                text: "Error message:\n" + ping.err_msg
                color: "white"
                font.family: "unicode"
                font.pointSize: 8
            }
        }
    }
}
