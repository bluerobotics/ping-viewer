import QtQuick 2.0
import Qt.labs.settings 1.0
import QtQuick.Layouts 1.3

Item {
    id: root
    x: margin
    width: mainLayout.width
    height: mainLayout.height

    property real value: 0
    property string units: "m"
    property real precision: 2
    property int margin: 10
    property real depth: -1
    property real strength: -1
    property real confidence: 0
    property real columnConfidence: 0
    property real columnDepth: 0
    property int parentWidth: 0
    property int parentHeight: 0

    // Take care of the item position while resizing parent
    Connections {
        target: parent
        onWidthChanged: {
            if (parentWidth) {
                var scaleW = parent.width/parentWidth
                if (scaleW) {
                    root.x *= scaleW
                    parentWidth = parent.width
                }
                return
            }
            parentWidth = parent.width
        }
        onHeightChanged: {
            if (parentHeight) {
                var scaleH = parent.height/parentHeight
                if (scaleH) {
                    root.y *= scaleH
                    parentHeight = parent.height
                }
                return
            }
            parentHeight = parent.height
        }
    }

    ColumnLayout {
        id: mainLayout
        RowLayout {
            Text {
                id: readout
                text: value.toFixed(precision) + units
                color: 'white'
                font.family: "Arial"
                font.pointSize: 48
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
                font.pointSize: readout.font.pointSize
                font.bold: true
            }
        }

        RowLayout {
            Text {
                id: textColumnDepth
                text: transformValue(columnDepth, precision) + units
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
                id: textColumnConfidence
                text: transformValue(columnConfidence, 0) + '%'
                visible: typeof(strength) == "number"
                color: 'white'
                font.family: "Arial"
                font.pointSize: 14
                font.bold: true
            }
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
                text: transformValue(strength*100, 0) + '%'
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
        if(y <= 0 || y + height > parent.height) {
            y = margin
        }
        if(x <= 0 || x + width > parent.width) {
            x = margin
        }
    }
}