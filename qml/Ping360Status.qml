import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Item {
    id: root
    anchors.fill: parent

    property var marginPix: 10
    property var ping: null

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
                    "FW: " + ping.firmware_version_major + "." + ping.firmware_version_minor + "." + ping.firmware_version_patch,
                    "SRC: " + ping.srcId + " DST: " + ping.dstId,
                    "Connection: " + ping.link.configuration.string,
                    "Range (m): " + ping.range.toFixed(2),
                    "Sample period (ticks): " + ping.sample_period,
                    "Sample period (ns): " + ping.sample_period*25,
                    "Number of samples (#): " + ping.number_of_points,
                    "Profile frequency (Hz): " + ping.profileFrequency.toFixed(2),
                    "Ping (#): " + ping.ping_number,
                    "Angle (grad): " + ping.angle,
                    "Angle Offset (grad): " + ping.angle_offset,
                    "Transmit frequency (kHz): " + ping.transmit_frequency,
                    "Transmit duration (μs): " + ping.transmit_duration,
                    "Transmit duration maximum (μs): " + ping.transmitDurationMax,
                    "Gain (setting): " + ping.gain_setting,
                    "Speed of sound (m/s): " + ping.speed_of_sound,
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
