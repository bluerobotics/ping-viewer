import QtGraphicalEffects 1.0
import QtQuick 2.12
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4 as QC1
import QtQuick.Layouts 1.3
import QtQuick.Shapes 1.12
import Qt.labs.settings 1.0

import GradientScale 1.0
import WaterfallPlot 1.0
import PolarPlot 1.0

import DeviceManager 1.0
import FileManager 1.0
import SettingsManager 1.0
import StyleManager 1.0
import Util 1.0

Item {
    id: root
    property alias displaySettings: displaySettings

    anchors.fill: parent
    property var ping: DeviceManager.primarySensor

    /** Some visual elements doesn't need to be refreshed in sensor sample frequency
      * Improves UI performance
      */

    Timer {
        interval: 50
        running: true
        repeat: true
        onTriggered: {
            shapeSpinner.angle = (ping.angle + 0.25)*180/200
            if(chart.visible) {
                chart.draw(ping.data, ping.range, 0)
            }
        }
    }

    Connections {
        target: ping

        /** Ping360 does not handle auto range/scale
         *  Any change in scale is a result of user input
         */
        onRangeChanged: {
            clear()
        }

        onSectorSizeChanged : {
            clear()
        }

        onDataChanged: {
            waterfall.draw(ping.data, ping.angle, 0, ping.range, ping.angular_speed, ping.sectorSize)
        }
    }

    onWidthChanged: {
        if(chart.Layout.minimumWidth === chart.width) {
            waterfall.parent.width = width - chart.width
        }
    }

    QC1.SplitView {
        orientation: Qt.Horizontal
        anchors.fill: parent

        Item {
            id: waterfallParent
            Layout.fillHeight: true
            Layout.fillWidth: true

            PolarPlot {
                id: waterfall
                /*
                height: Math.min(ping.sectorSize > 180 ? parent.height : parent.height*2, parent.width*scale)
                width: height
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: ping.sectorSize > 180 ? parent.verticalCenter : parent.bottom
                */
                anchors.fill: parent

                property var scale: ping.sectorSize >= 180 ? 1 : 0.8/Math.sin(ping.sectorSize*Math.PI/360)
                //property variant src: waterfall
                property var angle: 1

                property bool verticalFlip: false
                property bool horizontalFlip: false

                //vertexShader: "qrc:/opengl/polarplot/vertex.glsl"
                //fragmentShader: "qrc:/opengl/polarplot/fragment.glsl"

                Text {
                    id: northText
                    text: "N"
                    color: "red"
                    font.pixelSize: 20
                    font.bold: true
                    visible: northArrow.visible
                    y: -height
                    anchors.horizontalCenter: parent.horizontalCenter

                    transform: Rotation {
                        origin.x: northText.width/2
                        origin.y: northText.height/2
                        axis { x:0; y:0; z:1 }
                        angle: ping.heading*180/200
                    }
                }

                Canvas {
                    id: northArrow
                    width: 12
                    height: 20
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    onWidthChanged: requestPaint()
                    visible: ping && ping.sectorSize > 180 && ping.heading != 0
                    clip: false
                    onPaint: {
                        var ctx = getContext("2d")

                        ctx.fillStyle = "red"
                        ctx.strokeStyle = "black"

                        ctx.beginPath()
                        ctx.moveTo(width/2, 0)
                        ctx.lineTo(width, height)
                        ctx.lineTo(0, height)
                        ctx.lineTo(width/2, 0)
                        ctx.stroke()

                        ctx.fill()
                    }
                }

                // Spinner that shows the head angle
                Shape {
                    id: shapeSpinner
                    opacity: 1
                    anchors.centerIn: parent
                    vendorExtensionsEnabled: false
                    property var angle: 0

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

                Shape {
                    //visible: waterfall.containsMouse
                    anchors.centerIn: parent
                    opacity: 0.5
                    ShapePath {
                        strokeWidth: 3
                        strokeColor: StyleManager.secondaryColor
                        startX: 0
                        startY: 0
                        //TODO: This need to be updated in sensor integration
                        PathLine {
                            property real angle: 0//-Math.atan2(waterfall.mousePos.x - waterfall.width/2, waterfall.mousePos.y - waterfall.height/2) + Math.PI/2
                            x: waterfall.width*Math.cos(angle)/2
                            y: waterfall.height*Math.sin(angle)/2
                        }
                    }
                }

                transform: Rotation {
                    origin.x: waterfall.width/2
                    origin.y: waterfall.height/2
                    axis { x: waterfall.verticalFlip; y: waterfall.horizontalFlip; z: ping.sectorSize > 180}
                    angle: -ping.heading*180/200
                }
            }

            Text {
                id: mouseReadout
                //visible: waterfall.containsMouse
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
                    text: calcAngleFromFlips(waterfall.mouseSampleAngle) + "º"
                    color: StyleManager.secondaryColor

                    function calcAngleFromFlips(angle) {
                        var value = waterfall.mouseSampleAngle
                        if(waterfall.verticalFlip && waterfall.horizontalFlip) {
                            value = (360 + 270 - value) % 360
                            return transformValue(value)
                        }

                        if(waterfall.verticalFlip) {
                            value = (360 + 180 - value) % 360
                            return transformValue(value)
                        }

                        if(waterfall.horizontalFlip) {
                            value = 360 - value
                            return transformValue(value)
                        }

                        return transformValue(value)
                    }
                }
            }

            PolarGrid {
                id: polarGrid
                anchors.fill: waterfall
                angle: ping.sectorSize
                maxDistance: waterfall.maxDistance
            }

            GradientScale {
                width: 100
                height: 10
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                //waterfallGradient: waterfall.waterfallGradient
            }
        }

        Chart {
            id: chart
            Layout.fillHeight: true
            Layout.maximumWidth: 250
            Layout.preferredWidth: 100
            Layout.minimumWidth: 75
            // By default for ping360 the chart starts from the bottom
            flip: true
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
            id: settingsLayout
            anchors.fill: parent
            columns: 5
            rowSpacing: 5
            columnSpacing: 5
            property bool isFullCircle: true

            // Sensor connections should be done inside component
            // Components are local '.qml' descriptions, so it's not possible get outside connections
            Connections {
                target: ping
                onSectorSizeChanged: {
                    settingsLayout.isFullCircle = ping.sectorSize == 360
                }
            }

            CheckBox {
                id: horizontalFlipChB
                text: "Head down"
                checked: false
                Layout.columnSpan: 5
                Layout.fillWidth: true
                onCheckStateChanged: {
                    waterfall.horizontalFlip = checkState
                }
            }

            CheckBox {
                id: flipAScan
                text: "Flip A-Scan"
                checked: false
                Layout.columnSpan: 5
                Layout.fillWidth: true
                onCheckStateChanged: {
                    chart.flip = !checkState
                }
            }

            CheckBox {
                id: smoothDataChB
                text: "Smooth Data"
                checked: true
                Layout.columnSpan: 5
                Layout.fillWidth: true
                onCheckStateChanged: {
                    waterfall.smooth = checkState
                }
            }

            CheckBox {
                id: antialiasingDataChB
                text: "Antialiasing"
                property bool isMac: Util.isMac()
                checked: !isMac
                visible: !isMac || SettingsManager.debugMode
                Layout.columnSpan: 5
                Layout.fillWidth: true
                onVisibleChanged: {
                    if(!visible && isMac) {
                        checked = false
                    }
                }
                onCheckStateChanged: {
                    waterfall.antialiasing = checkState
                }
            }

            CheckBox {
                id: removeAScanChB
                text: "A-Scan"
                checked: true
                Layout.columnSpan:  5
                Layout.fillWidth: true
                onCheckStateChanged: {
                    chart.visible = checkState
                }
            }

            Label {
                text: "Plot Theme:"
            }

            PingComboBox {
                id: plotThemeCB
                Layout.columnSpan: 4
                Layout.fillWidth: true
                Layout.minimumWidth: 200
                model: waterfall.themes
                onCurrentTextChanged: waterfall.theme = currentText
            }

            Settings {
                category: "Ping360Visualizer"
                property alias flipAScanState: flipAScan.checkState
                property alias plotThemeIndex: plotThemeCB.currentIndex
                property alias removeAScanState:removeAScanChB.checkState
                property alias smoothDataState: smoothDataChB.checkState
                property alias waterfallAntialiasingData: antialiasingDataChB.checkState
            }
        }
    }
}
