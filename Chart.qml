import QtQuick 2.0
import QtCharts 2.2

Item {
    id: root

    ChartView {
        id: chart
        antialiasing: true
        animationDuration: 10
        //width: root.width
        //height: root.height
        width: root.height
        height: root.width
        anchors { horizontalCenter: parent.horizontalCenter
        verticalCenter: parent.verticalCenter
        }
        transform: Rotation { origin.x: height/2; origin.y: width/2; angle: 90}

        ValueAxis {
            id: axisX
            titleText: qsTr("Distance [m]")
            labelFormat: "%.1f"
            tickCount: 8
            min: 0
            max: 200
        }

        ValueAxis {
            id: axisY
            titleText: qsTr("Intensity [dB]")
            labelFormat: "%.1f"
            tickCount: 8
            min: 0
            max: 255
        }

        LineSeries {
            id: serie
            name: "Ping"
            axisX: axisX
            axisY: axisY
        }

        Timer {
            interval: 500; running: true; repeat: true
            onTriggered: {
                for (var i=0; i<200; i++) {
                    serie.append(i, 255*Math.random())
                }

                if(serie.count > 200) {
                    serie.removePoints(0, serie.count - 200)
                }
                print(serie.count, root.x, root.y)
            }
        }
    }
}