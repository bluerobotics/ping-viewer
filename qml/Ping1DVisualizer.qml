import QtGraphicalEffects 1.0
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4 as QC1
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0
import Waterfall 1.0

Item {
    id: visualizer
    property alias waterfallItem: waterfall
    property var protocol

    onWidthChanged: {
        if(chart.Layout.minimumWidth === chart.width) {
            waterfall.width = width - chart.width
        }
    }

    function draw(points, depth, confidence, initialPoint) {
        waterfall.draw(points, depth, confidence, initialPoint)
        chart.draw(points, depth, initialPoint)
    }

    function setDepth(depth) {
        readout.value = depth
    }

    function setConfidence(perc) {
        readout.confidence = perc
    }

    QC1.SplitView {
        orientation: Qt.Horizontal
        anchors.fill: parent

        Waterfall {
            id: waterfall
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: 350
            Layout.minimumWidth: 350
            onMouseMove: {
                readout.depth = Math.min(waterfall.mouseDepth, 0)
                readout.strength = Math.min(waterfall.mouseStrength, 0)
                readout.columnDepth = Math.min(waterfall.mouseColumnDepth, 0)
                readout.columnConfidence = Math.min(waterfall.mouseColumnConfidence, 0)
            }
        }

        Chart {
            id: chart
            Layout.fillHeight: true
            Layout.maximumWidth: 250
            Layout.preferredWidth: 100
            Layout.minimumWidth: 75
            maxDepthToDraw: waterfall.maxDepthToDraw
            minDepthToDraw: waterfall.minDepthToDraw
        }

        Settings {
            property alias chartWidth: chart.width
        }
    }

    ValueReadout {
        id: readout
    }
}
