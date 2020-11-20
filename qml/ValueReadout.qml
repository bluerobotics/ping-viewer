import Qt.labs.settings 1.0
import QtQuick 2.15
import QtQuick.Layouts 1.3
import SettingsManager 1.0

Item {
    id: root

    property real value: 0
    property string units: SettingsManager.distanceUnits["distance"]
    property real scalar: SettingsManager.distanceUnits["distanceScalar"]
    property real precision: 2
    property int margin: 10
    property real confidence: 0
    property int parentWidth: 0
    property int parentHeight: 0
    property var _POSITION: Object.freeze({
        "X": 1,
        "Y": 2
    })

    function confidenceToColor(confidence) {
        return Qt.rgba(2 * (1 - confidence / 100), 2 * confidence / 100, 0);
    }

    function transformValue(value, precision) {
        return typeof (value) == "number" ? value.toFixed(precision) : value + " ";
    }

    function clamp(x, min, max) {
        return x < min ? min : (x > max ? max : x);
    }

    function checkPosition(positionType) {
        // Check if parent is visible and correct initialized
        if (!root.parent.visible || root.parent.width === 0 || root.parent.height === 0)
            return ;

        // Calculate the maximum available position
        // Check if the window can contain the item
        // Test if value is arealdy in use
        if (positionType === _POSITION.X) {
            let maxSize = parent.width - root.width - margin;
            if (maxSize <= 0 || margin >= maxSize)
                return ;

            let newValue = clamp(x, margin, maxSize);
            if (newValue !== x)
                x = newValue;

        } else if (positionType === _POSITION.Y) {
            let maxSize = parent.height - root.height - margin;
            if (maxSize <= 0 || margin >= maxSize)
                return ;

            let newValue = clamp(y, margin, maxSize);
            if (newValue !== y)
                y = newValue;

        }
    }

    width: mainLayout.width
    height: mainLayout.height
    // Component is used when the application is started for the first time in a computer
    // since X and Y is not emited
    Component.onCompleted: {
        checkPosition(_POSITION.X);
        checkPosition(_POSITION.Y);
    }
    // X and Y signals are necessary, since onCompleted is emitted before the application is
    // correct initialized
    onXChanged: checkPosition(_POSITION.X)
    onYChanged: checkPosition(_POSITION.Y)

    // Take care of the item position while resizing parent
    Connections {
        target: parent
        function onWidthChanged() {
            if (parentWidth) {
                var scaleW = parent.width / parentWidth;
                if (scaleW) {
                    root.x *= scaleW;
                    parentWidth = parent.width;
                }
                return ;
            }
            parentWidth = parent.width;
        }
        function onHeightChanged() {
            if (parentHeight) {
                var scaleH = parent.height / parentHeight;
                if (scaleH) {
                    root.y *= scaleH;
                    parentHeight = parent.height;
                }
                return ;
            }
            parentHeight = parent.height;
        }
    }

    ColumnLayout {
        id: mainLayout

        Text {
            id: readout

            text: (scalar * value).toFixed(precision) + units
            color: confidenceToColor(confidence)
            font.family: "Arial"
            font.pointSize: 48
            font.bold: true
            style: Text.Outline
            styleColor: "black"

            Text {
                id: confidenceText

                x: readout.width - width
                y: readout.height * 4 / 5
                text: "Confidence: " + transformValue(confidence) + "%"
                visible: typeof (confidence) == "number"
                color: confidenceToColor(confidence)
                font.family: "Arial"
                font.pointSize: readout.font.pointSize / 3
                font.bold: true
                style: Text.Outline
                styleColor: "black"
                anchors.margins: 0
            }

        }

    }

    MouseArea {
        anchors.fill: parent
        drag.target: parent
        z: 1
        //acceptedButtons: Qt.LeftButton
        onPressed: {
            cursorShape = Qt.ClosedHandCursor;
            previousPosition = Qt.point(mouseX, mouseY);
        }
        onReleased: {
            cursorShape = Qt.OpenHandCursor;
        }
        onWheel: {
            readout.font.pointSize += wheel.angleDelta.y / 120;
            if (readout.font.pointSize < 15)
                readout.font.pointSize = 15;

            if (readout.font.pointSize > 100)
                readout.font.pointSize = 100;

        }
    }

    Settings {
        id: settings

        property alias valueReadoutX: root.x
        property alias valueReadoutY: root.y
        property alias readoutFontSize: readout.font.pointSize
    }

}
