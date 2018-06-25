import QtQuick 2.0
import QtCharts 2.2
import Util 1.0

Item {
    id: root
    anchors.margins: 0
    transform: Rotation { origin.x: width/2; origin.y: height/2; angle: 90}

    property real maxDepthToDraw: 0
    property real minDepthToDraw: 0

    function correctChartSize() {
        chart.height = width
        // plotArea.x is a private margin in ChartView
        // it's not possible to set as 0
        chart.width = height + 2*chart.plotArea.x
    }

    function draw(points, depth, initPos) {
        chart.draw(points, depth, initPos)
        correctChartSize()
    }
    onWidthChanged: correctChartSize()
    onHeightChanged: correctChartSize()

    ChartView {
        id: chart
        width: root.height
        height: root.width

        antialiasing: true
        legend.visible: false
        backgroundColor: 'transparent'
        plotAreaColor: 'transparent'

        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }

        backgroundRoundness: 0

        margins.top: 0
        margins.bottom: 0
        margins.left: 0
        margins.right: 0
        anchors.margins: 0

        ValueAxis {
            id: axisX
            //titleText: qsTr("Distance [m]")
            visible: false
            lineVisible: false
            gridVisible: false
            labelsAngle: -90
            labelsVisible: false
            labelFormat: "%.1f"
            tickCount: 3
            min: 0
            max: 300
        }

        ValueAxis {
            id: axisY
            //titleText: qsTr("Intensity [dB]")
            visible: false
            lineVisible: false
            gridVisible: false
            labelsAngle: -90
            labelsVisible: false
            labelFormat: "%.1f"
            tickCount: 2
            min: -1
            max: 1
        }

        LineSeries {
            id: serie
            useOpenGL: true
            pointLabelsVisible: false
            pointsVisible: false
            axisX: axisX
            axisY: axisY
            color: 'lime'
        }

        LineSeries {
            id: serieInv
            useOpenGL: true
            pointLabelsVisible: false
            pointsVisible: false
            axisX: axisX
            axisY: axisY
            color: 'lime'
        }

        function draw(points, depth, initPost) {
            if (points) {
                Util.update(serie, points, initPost, depth, minDepthToDraw, maxDepthToDraw, 1)
                Util.update(serieInv, points, initPost, depth, minDepthToDraw, maxDepthToDraw, -1)
                return
            }

            serie.clear()
            serieInv.clear()
            for (var i=0; i<200; i++) {
                var pt = 255*Math.random()
                serie.append(i, pt)
                serieInv.append(i, -pt)
            }
        }

        Component.onCompleted: {
            for(var id in axes) {
                axes[id].visible = false
            }
        }
    }
}
