import QtQuick 2.0
import Qt.labs.settings 1.0
import QtQuick.Layouts 1.3

Item {
    id: root
    property var value: 0
    property var units: "m"
    property var precision: 1
    property var margin: 10
    property var depth: -1
    property var strength: -1
    property var confidence: 0
    x: margin

    width: mainLayout.width
    height: mainLayout.height

    ColumnLayout {
        id: mainLayout
        Text {
            id: readout
            text: value.toFixed(precision) + units
            color: 'white'
            font.family: "Arial"
            font.pointSize: 48
            font.bold: true
        }
        RowLayout {
            Text {
                id: depthText
                text: transformValue(depth, precision) + units
                visible: typeof(strength) == "number"
                color: 'white'
                font.family: "Arial"
                font.pointSize: 14
                font.bold: true
            }

            Rectangle {
                Layout.fillWidth: true
                color: "transparent"
            }

            Text {
                id: strengthText
                text: transformValue(strength, 2) + 'dB'
                visible: typeof(strength) == "number"
                color: 'white'
                font.family: "Arial"
                font.pointSize: 14
                font.bold: true
            }

            Rectangle {
                Layout.fillWidth: true
                color: "transparent"
            }

            Text {
                id: confidenceText
                text: transformValue(confidence) + '%'
                visible: typeof(strength) == "number"
                color: 'white'
                font.family: "Arial"
                font.pointSize: 14
                font.bold: true
            }
        }
    }

    function transformValue(value, precision) {
        return typeof(value) == "number" ? value.toFixed(precision) : value + ' '
    }

    MouseArea {
        anchors.fill: parent;
        z: 1
        //acceptedButtons: Qt.LeftButton
        property var previousPosition
        onPressed: {
            cursorShape = Qt.ClosedHandCursor
            previousPosition = Qt.point(mouseX, mouseY)
        }
        onReleased: {
            cursorShape = Qt.OpenHandCursor
        }
        onPositionChanged: {
            var dx = mouseX - previousPosition.x
            var dy = mouseY - previousPosition.y
            root.x += dx
            root.y += dy
        }
        onWheel: {
            readout.font.pointSize += wheel.angleDelta.y/120
            if (readout.font.pointSize < 15)  {
                readout.font.pointSize = 15
            }
            if (readout.font.pointSize > 100)  {
                readout.font.pointSize = 100
            }
        }
    }

    Settings {
        id: settings
        property alias valueReadoutX: root.x
        property alias valueReadoutY: root.y
        property alias readoutFontSize: readout.font.pointSize
    }


    Component.onCompleted: {
        if(root.y == 0) {
            root.y = parent.height - height - margin
        }
    }
}