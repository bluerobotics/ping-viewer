import QtGraphicalEffects 1.0
import QtQuick 2.12
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4 as QC1
import QtQuick.Layouts 1.3
import QtQuick.Shapes 1.12
import Qt.labs.settings 1.0
import WaterfallPlot 1.0
import PolarPlot 1.0

import DeviceManager 1.0
import FileManager 1.0
import SettingsManager 1.0
import StyleManager 1.0

Item {
    id: root
    property alias displaySettings: displaySettings
    anchors.fill: parent

    Connections {
        property var ping: DeviceManager.primarySensor
        target: ping

        onDataChanged: {
            shapeSpinner.angle = (ping.angle + 0.25)*180/200
            root.draw(ping.data, ping.angle, 0, 100)
        }
    }

    onWidthChanged: {
        if(chart.Layout.minimumWidth === chart.width) {
            waterfall.parent.width = width - chart.width
        }
    }

    function draw(points, angle, initialPoint, length) {
        waterfall.draw(points, angle, initialPoint, length)
        chart.draw(points, length + initialPoint, initialPoint)
    }

    QC1.SplitView {
        orientation: Qt.Horizontal
        anchors.fill: parent

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            PolarPlot {
                id: waterfall
                height: Math.min(parent.height, parent.width)
                width: height
                anchors.centerIn: parent

                Shape {
                    visible: waterfall.containsMouse
                    anchors.centerIn: parent
                    opacity: 0.5
                    ShapePath {
                        strokeWidth: 3
                        strokeColor: StyleManager.secondaryColor
                        startX: 0
                        startY: 0
                        //TODO: This need to be updated in sensor integration
                        PathLine {
                            property real angle: -Math.atan2(waterfall.mousePos.x - waterfall.width/2, waterfall.mousePos.y - waterfall.height/2) + Math.PI/2
                            x: waterfall.width*Math.cos(angle)/2
                            y: waterfall.height*Math.sin(angle)/2
                        }
                    }
                }

                // Spinner that shows the head angle
                Shape {
                    id: shapeSpinner
                    opacity: 1
                    anchors.centerIn: parent
                    vendorExtensionsEnabled: false
                    property var angle: 0
                    property var centerX: waterfall.width/2
                    property var centerY: waterfall.height/2
                    property var radius: waterfall.width/2

                    ShapePath {
                        id: shapePathSpinner
                        strokeWidth: 2
                        strokeColor: StyleManager.secondaryColor

                        PathLine {
                            x: waterfall.width*Math.cos(shapeSpinner.angle*Math.PI/180 - Math.PI/2)/2
                            y: waterfall.height*Math.sin(shapeSpinner.angle*Math.PI/180 - Math.PI/2)/2
                        }
                    }
                }

                Text {
                    id: mouseReadout
                    visible: waterfall.containsMouse
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 5
                    font.bold: true
                    font.family: "Arial"
                    font.pointSize: 15
                    text: (waterfall.mouseSampleDistance*SettingsManager.distanceUnits['distanceScalar']).toFixed(2) + SettingsManager.distanceUnits['distance']
                    color: StyleManager.secondaryColor

                    Text {
                        id: mouseConfidenceText
                        visible: typeof(waterfall.mouseSampleAngle) == "number"
                        anchors.top: parent.bottom
                        anchors.margins: 5
                        font.bold: true
                        font.family: "Arial"
                        font.pointSize: 15
                        text: transformValue(waterfall.mouseSampleAngle) + "º"
                        color: StyleManager.secondaryColor
                    }
                }

                PolarGrid {
                    id: polarGrid
                    anchors.fill: parent
                    maxDistance: waterfall.maxDistance
                }
            }
        }

        Chart {
            id: chart
            Layout.fillHeight: true
            Layout.maximumWidth: 250
            Layout.preferredWidth: 100
            Layout.minimumWidth: 75
        }

        Settings {
            property alias chartWidth: chart.width
        }
    }

    function transformValue(value, precision) {
        return typeof(value) == "number" ? value.toFixed(precision) : value + ' '
    }

    function captureVisualizer() {
        waterfall.grabToImage(function(result) {
            print("Grab waterfall image callback.")
            print(FileManager.createFileName(FileManager.Pictures))
            result.saveToFile(FileManager.createFileName(FileManager.Pictures))
        })
    }

    function clear() {
        waterfall.clear()
    }

    function handleShortcut(key) {
        return false
    }

    Component {
        id: displaySettings
        GridLayout {
            anchors.fill: parent
            columns: 5
            rowSpacing: 5
            columnSpacing: 5

            CheckBox {
                id: smoothDataChB
                text: "Smooth Data"
                checked: true
                Layout.columnSpan:  5
                Layout.fillWidth: true
                onCheckStateChanged: {
                    waterfall.smooth = checkState
                }
            }

            CheckBox {
                id: antialiasingDataChB
                text: "Antialiasing"
                checked: true
                Layout.columnSpan:  5
                Layout.fillWidth: true
                onCheckStateChanged: {
                    waterfall.antialiasing = checkState
                }
            }

            Label {
                text: "Plot Theme:"
            }

            ComboBox {
                id: plotThemeCB
                Layout.columnSpan:  4
                Layout.fillWidth: true
                Layout.minimumWidth: 200
                model: waterfall.themes
                onCurrentTextChanged: waterfall.theme = currentText
            }

            Settings {
                category: "Ping360Visualizer"
                property alias plotThemeIndex: plotThemeCB.currentIndex
                property alias smoothDataState: smoothDataChB.checkState
                property alias waterfallAntialiasingData: antialiasingDataChB.checkState
            }
        }
    }
}
