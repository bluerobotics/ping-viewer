import QtQuick 2.0

Item {
    property var value: 0
    property var units: "m"
    property var precision: 1

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
}
