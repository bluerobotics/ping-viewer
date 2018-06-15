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

    function draw(points, depth, confidence) {
        waterfall.draw(points, depth, confidence)
        chart.draw(points, depth)
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
                readout.depth = waterfall.mouseDepth < 0 ? '-' : waterfall.mouseDepth
                readout.strength = waterfall.mouseStrength  < 0 ? '-' : waterfall.mouseStrength
                readout.columnDepth = waterfall.mouseColumnDepth < 0 ? '-' : waterfall.mouseColumnDepth
                readout.columnConfidence = waterfall.mouseColumnConfidence < 0 ? '-' : waterfall.mouseColumnConfidence
            }
        }

        Chart {
            id: chart
            Layout.fillHeight: true
            Layout.maximumWidth: 250
            Layout.preferredWidth: 100
            Layout.minimumWidth: 75
            maxDepthToDraw: waterfall.maxDepthToDraw
        }

        Settings {
            property alias chartWidth: chart.width
        }
    }

    ValueReadout {
        id: readout
    }
}
