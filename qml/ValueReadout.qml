import QtQuick 2.7
import Qt.labs.settings 1.0
import QtQuick.Layouts 1.3

import SettingsManager 1.0

Item {
    id: root
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
        anchors.fill: parent
        drag.target: parent
        z: 1
        //acceptedButtons: Qt.LeftButton
        onPressed: {
            cursorShape = Qt.ClosedHandCursor
            previousPosition = Qt.point(mouseX, mouseY)
        }
        onReleased: {
            cursorShape = Qt.OpenHandCursor
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

    function clamp(x, min, max) {
        return x < min ? min : (x > max ? max : x)
    }

    function checkPosition() {
        // Check if parent is visible and correct initialized
        if(!root.parent.visible || root.parent.width === 0 || root.parent.height === 0) {
            return
        }

        x = clamp(x, margin, parent.width - root.width - margin)
        y = clamp(y, margin, parent.height - root.height - margin)
    }

    // Component is used when the application is started for the first time in a computer
    // since X and Y is not emited
    Component.onCompleted: checkPosition()
    // X and Y signals are necessary, since onCompleted is emitted before the application is
    // correct initialized
    onXChanged: checkPosition()
    onYChanged: checkPosition()
}
