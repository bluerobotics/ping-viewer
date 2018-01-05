import QtGraphicalEffects 1.0
import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Controls 1.4 as QC1
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

QC1.SplitView {

    id: visualizer
    orientation: Qt.Horizontal

    Waterfall {
        id: waterfall
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.minimumWidth: 150
    }

    Chart {
        id: chart
        Layout.fillHeight: true
        Layout.preferredWidth: 350
        Layout.minimumWidth: 350
    }

    function draw(points) {
        waterfall.draw(points)
        chart.draw(points)
    }

    Settings {
        property alias chartWidth: chart.width
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
        }
    }
}
