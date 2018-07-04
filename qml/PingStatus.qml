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
        Column {
            id: innerCol
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: marginPix
            Row {
                Text {
                    text: "FW: " + ping.fw_version_major + "." + ping.fw_version_minor
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "SRC: " + ping.srcId + " DST: " + ping.dstId
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Distance (mm): " + ping.distance
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Auto (bool): " + ping.mode_auto
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Start (mm): " + ping.start_mm
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Length (mm): " + ping.length_mm
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Ping (#): " + ping.ping_number
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Pulse durantion (μs): " + ping.pulse_usec
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Ping interval (ms): " + ping.msec_per_ping
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Gain (index): " + ping.gain_index
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Confidence (%): " + ping.confidence
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Speed of sound (mm/s): " + ping.speed_of_sound
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Processor temperature (C): " + (ping.processor_temperature / 100).toFixed(1)
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            // N/A for now
//            Row {
//                Text {
//                    text: "PCB temperature (C): " + ping.pcb_temperature
//                    color: "white"
//                    font.family: "unicode"
//                    font.pointSize: 8
//                }
//            }
            Row {
                Text {
                    text: "Board voltage (V): " + (ping.board_voltage / 1000).toFixed(2)
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Ascii text:\n" + ping.ascii_text
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Error message:\n" + ping.err_msg
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
        }
    }
}
