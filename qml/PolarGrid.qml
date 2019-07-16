import QtQuick 2.12
import QtQuick.Shapes 1.12
import SettingsManager 1.0

Item {
    id: root
    width: 400
    height: width
    property var maxDistance: 100e3
    property bool enableText: true
    Repeater {
        id: repeater
        anchors.fill: parent
        property var numberOfRings: 4
        property var angle: 360
        property var radius: root.height/2
        property var centerX: root.width/2
        property var centerY: root.height/2
        // Distance text properties
        property string units: SettingsManager.distanceUnits['distance']
        property real scalar: SettingsManager.distanceUnits['distanceScalar']
        model: Math.floor(360/angle)*numberOfRings

        function formatDistance(distance) {
            // Change precision based in distance
            var precision = 0
            if(2e3 < root.maxDistance && root.maxDistance < 5e3) {
                precision = 1
            } else if(root.maxDistance < 2e3) {
                precision = 2
            }
            return (distance*scalar/1e3).toFixed(precision) + units
        }

        delegate: Text {
            font.pixelSize: 15
            visible: root.enableText
            // Calculate the angle and radius for each item
            property var itemRadian: (index%Math.floor(360/repeater.angle))*repeater.angle*Math.PI/180
            property var itemRadius: repeater.radius*(1 + Math.floor(index/Math.floor(360/repeater.angle)))/repeater.numberOfRings
            text: repeater.formatDistance(root.maxDistance*(1 + Math.floor(index/Math.floor(360/repeater.angle)))/repeater.numberOfRings)
            //The position should change for each visible angle
            x: (1.2*width/2 + itemRadius)*Math.cos(itemRadian) + Math.abs((1.2*width/2)*Math.sin(itemRadian)) + repeater.centerX - width/2
            y: (1.1*height/2 + itemRadius)*Math.sin(itemRadian) - Math.abs((1.1*height/2)*Math.cos(itemRadian)) + repeater.centerY - height/2
            style: Text.Outline
            styleColor: "white"
        }
    }

    Shape {
        anchors.fill: parent
        vendorExtensionsEnabled: false

        // polar axis
        ShapePath {
            id: shapePathPolar
            fillColor: "transparent"
            strokeColor: "white"
            strokeWidth: 1
            property var centerX: root.width/2
            property var centerY: root.height/2
            PathAngleArc {
                centerX: shapePathPolar.centerX
                centerY: shapePathPolar.centerY
                radiusX: root.width/2
                radiusY: root.height/2
                startAngle: 0
                sweepAngle: 360
            }

            PathAngleArc {
                centerX: shapePathPolar.centerX
                centerY: shapePathPolar.centerY
                radiusX: (3/4)*root.width/2
                radiusY: (3/4)*root.height/2
                startAngle: 0
                sweepAngle: 360
            }

            PathAngleArc {
                centerX: shapePathPolar.centerX
                centerY: shapePathPolar.centerY
                radiusX: (2/4)*root.width/2
                radiusY: (2/4)*root.height/2
                startAngle: 0
                sweepAngle: 360
            }

            PathAngleArc {
                centerX: shapePathPolar.centerX
                centerY: shapePathPolar.centerY
                radiusX: (1/4)*root.width/2
                radiusY: (1/4)*root.height/2
                startAngle: 0
                sweepAngle: 360
            }
        }
    }

    Canvas {
        anchors.fill: parent
        opacity: 0.8
        property var angle: 45

        onPaint: {
            var centerX = root.width/2
            var centerY = root.height/2
            var radius = root.height/2 - shapePathPolar.strokeWidth
            var ctx = getContext("2d")
            ctx.lineWidth = 1
            ctx.strokeStyle = "white"
            ctx.beginPath()

            for(var i=0; i*angle < 360; i++){
                ctx.moveTo(centerX, centerY);
                var angleRadians = i*angle*Math.PI/180;
                ctx.lineTo(radius*Math.cos(angleRadians) + centerX, radius*Math.sin(angleRadians) + centerY);
            }

            ctx.closePath()
            ctx.stroke()
        }
    }
}
