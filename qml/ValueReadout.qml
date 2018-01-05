import QtQuick 2.0
import Qt.labs.settings 1.0

Item {
    id: root
    property var value: 0
    property var units: "m"
    property var precision: 1
    property var margin: 10
    x: margin
    y: parent.height - height - margin

    width: readout.width
    height: readout.height

    Text {
        id: readout
        text: value.toFixed(precision) + units
        color: 'white'
        font.family: "Arial"
        font.pointSize: 48
        font.bold: true
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
        property alias valueReadoutX: root.x
        property alias valueReadoutY: root.y
        property alias readoutFontSize: readout.font.pointSize
    }
}
