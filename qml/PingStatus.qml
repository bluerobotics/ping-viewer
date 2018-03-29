import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4 as QC1
import QtQuick.Layouts 1.3

Item {
    id: root

    property var ping

    anchors.fill: parent

    Rectangle {
        anchors.centerIn: parent

        color: "black"
        opacity: 0.75
        height: innerCol.height
        width: innerCol.width*1.2
        Column {
            id: innerCol
            anchors.centerIn: parent
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
                    text: "Distance: " + ping.distance
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Start: " + ping.start_mm
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Length: " + ping.length_mm
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Ping #: " + ping.ping_number
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Pulse usec: " + ping.pulse_usec
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Ping msec: " + ping.msec_per_ping
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Gain index: " + ping.gain_index
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
            Row {
                Text {
                    text: "Confidence: " + ping.confidence
                    color: "white"
                    font.family: "unicode"
                    font.pointSize: 8
                }
            }
        }
    }
}
