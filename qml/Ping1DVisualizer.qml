import QtGraphicalEffects 1.0
import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Controls 1.4 as QC1
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0
import Waterfall 1.0

Item {
    id: visualizer
    property var waterfallItem: waterfall
    property var protocol

    onWidthChanged: {
        if(chart.Layout.minimumWidth == chart.width) {
            waterfall.width = width - chart.width
        }
    }

    function draw(points) {
        waterfall.draw(points)
        chart.draw(points)
    }

    function setDepth(depth) {
        readout.value = depth
        readout.depth = waterfall.mouseDepth < 0 ? '-' : waterfall.mouseDepth
        readout.strength = waterfall.mouseStrength  < 0 ? '-' : waterfall.mouseStrength
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

            theme: PingSettings.plotTheme
            smooth: PingSettings.smoothIsEnable

            Component.onCompleted: {
                PingSettings.plotThemesAvailable = themes
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

    ValueReadout {
        id: readout
    }
}
