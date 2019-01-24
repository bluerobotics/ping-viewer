import QtQuick 2.7
import Qt.labs.settings 1.0
import QtQuick.Layouts 1.3

import SettingsManager 1.0

Item {
    id: root
    x: margin
    width: mainLayout.width
    height: mainLayout.height

    property real value: 0
    property string units: SettingsManager.distanceUnits['distance']
    property real scalar: SettingsManager.distanceUnits['distanceScalar']
    property real precision: 2
    property int margin: 10
    property real confidence: 0
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
        Text {
            id: readout
            width: textMetrics.width
            height: textMetrics.height
            text: (scalar*value).toFixed(precision) + units
            color: confidenceToColor(confidence)
            font.family: "Arial"
            font.pointSize: 48
            font.bold: true
            style: Text.Outline
            styleColor: "black"

             Text {
                id: confidenceText
                x: readout.width - width
                y: readout.height*4/5
                text: "Confidence: " + transformValue(confidence) + "%"
                visible: typeof(confidence) == "number"
                color: confidenceToColor(confidence)
                font.family: "Arial"
                font.pointSize: readout.font.pointSize/3
                font.bold: true
                style: Text.Outline
                styleColor: "black"
                anchors.margins: 0
            }
        }
    }

    function confidenceToColor(confidence) {
        return Qt.rgba(2*(1 - confidence/100), 2*confidence/100, 0)
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
