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
    property var plotTheme

    function draw(points) {
        waterfall.draw(points)
        chart.draw(points)
    }

    function setDepth(depth) {
        readout.value = depth
        readout.depth = waterfall.mouseDepth < 0 ? '-' : waterfall.mouseDepth
        readout.strength = waterfall.mouseStrength  < 0 ? '-' : waterfall.mouseStrength
    }

    QC1.SplitView {
        orientation: Qt.Horizontal
        anchors.fill: parent

        Waterfall {
            id: waterfall
            Layout.fillHeight: true
            Layout.preferredWidth: 350
            Layout.minimumWidth: 350
            theme: plotTheme
        }

        Chart {
            id: chart
            Layout.fillHeight: true
            Layout.preferredWidth: 350
            Layout.minimumWidth: 350
        }

        Settings {
            property alias waterfallWidth: waterfall.width
            property alias waterfallThemes: waterfall.themes
        }
    }

    ValueReadout {
        id: readout
    }

    Timer {
        id: testTimer
        interval: 40; running: true; repeat: true
        property var counter: 0
        onTriggered: {
            counter = counter + 1
            var points = []
            var numPoints = 200
            var stop1 = numPoints / 2.0 - 10*Math.sin(counter/10.0)
            var stop2 = 3 * numPoints / 5.0 + 6*Math.cos(counter/5.5)
            for (var i = 0; i < numPoints; i++) {
                var point
                if (i < stop1) {
                    point = 0.1 * Math.random()
                } else if (i < stop2) {
                    point = (-4 / Math.pow((stop2-stop1), 2)) * Math.pow((i - stop1 - ((stop2-stop1) / 2.0)), 2)  + 1
                } else {
                    point = 0.45 * Math.random();
                }
                points.push(point)
            }
            visualizer.draw(points)
            visualizer.setDepth(stop1 + (stop2-stop1) / 2.0)
        }
    }
}
